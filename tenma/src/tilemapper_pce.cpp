#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "util/TFreeSpace.h"
#include "util/TFileManip.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TParse.h"
#include "pce/PcePattern.h"
#include "pce/PcePalette.h"
#include "pce/PceTilemap.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <cstring>

using namespace std;
using namespace BlackT;
using namespace Pce;

// stupid and inefficient, but good enough for what we're doing
typedef vector<int> Blacklist;

struct UsedPaletteLineColorSet {
  UsedPaletteLineColorSet() {
    availableColors.resize(PcePaletteLine::numColors);
    for (int i = 0; i < PcePaletteLine::numColors; i++)
      availableColors[i] = false;
  }
  
  int numAvailableIndices() const {
    int count = 0;
    for (int i = 0; i < PcePaletteLine::numColors; i++)
      if (availableColors[i]) ++count;
    return count;
  }
  
  int nextAvailableIndex() const {
    for (int i = 0; i < PcePaletteLine::numColors; i++)
      if (availableColors[i]) return i;
    return -1;
  }
  
  int getAvailability(int index) const {
    return availableColors.at(index);
  }
  
  void unclaimIndex(int index) {
    if (index == 0) return;
    availableColors[index] = true;
  }
  
  void claimIndex(int index) {
    if (index == 0) return;
    availableColors[index] = false;
  }
  
  std::vector<bool> availableColors;
};

struct PaletteUseInfo {
  PaletteUseInfo() {
    lineInfo.resize(PcePalette::numPaletteLines);
  }
  
  UsedPaletteLineColorSet& getLineInfo(int index) {
    return lineInfo.at(index);
  }
  
  void readSpecifierString(std::string spec) {
    TBufStream ifs;
    ifs.writeString(spec);
    ifs.seek(0);
    while (!ifs.eof()) {
      int lineNum = TParse::matchInt(ifs);
      if (ifs.eof() || !TParse::checkChar(ifs, '/')) {
        // no specifier = enable entire palette
        for (int i = 0; i < PcePaletteLine::numColors; i++) {
          lineInfo[lineNum].unclaimIndex(i);
        }
      }
      else {
        while (TParse::checkChar(ifs, '/')) {
          TParse::matchChar(ifs, '/');
          int first = TParse::matchInt(ifs);
          int second = first;
          
          if (TParse::checkChar(ifs, '-')) {
            TParse::matchChar(ifs, '-');
            second = TParse::matchInt(ifs);
          }
          
          for (int i = first; i <= second; i++) {
            lineInfo[lineNum].unclaimIndex(i);
          }
        }
      }
    }
  }
  
  std::vector<UsedPaletteLineColorSet> lineInfo;
};

PaletteUseInfo paletteUseInfo;

/*int countColorsInArea(const TGraphic& grp, int x, int y, int w, int h) {
  int count = 0;
  
  std::map<TColor, bool> usedColors;
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      TColor color = grp.getPixel(x + i, y + j);
      if (usedColors.find(color) == usedColors.end()) {
        usedColors[color] = true;
        ++count;
      }
    }
  }
  
  return count;
} */

std::map<TColor, bool> getColorsInArea(
    const TGraphic& grp, int x, int y, int w, int h) {
  std::map<TColor, bool> usedColors;
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      TColor color = grp.getPixel(x + i, y + j);
      usedColors[color] = true;
    }
  }
  return usedColors;
}

int readIntString(const string& src, int* pos) {
  string numstr;
  while (*pos < src.size()) {
    // accept "x" for hex
    if (!isalnum(src[*pos]) && !(src[*pos] == 'x')) break;
    else {
      numstr += src[(*pos)++];
    }
  }
  
  if (*pos < src.size()) ++(*pos);
  
  return TStringConversion::stringToInt(numstr);
}

void readBlacklist(Blacklist& blacklist, const string& src) {
  int pos = 0;
  
  while ((pos < src.size())) {
    int next = readIntString(src, &pos);
    
    // check if this is a range
    if ((pos < src.size()) && (src[(pos - 1)] == '-')) {
      int next2 = readIntString(src, &pos);
      for (unsigned int i = next; i <= next2; i++) {
        blacklist.push_back(i);
      }
    }
    else {
      blacklist.push_back(next);
    }
  }
}

bool isBlacklisted(Blacklist blacklist, int value) {
  for (unsigned int i = 0; i < blacklist.size(); i++) {
    if (blacklist[i] == value) return true;
  }
  
  return false;
}

int processTile(const TGraphic& srcG, int x, int y,
                PcePalette& palette,
                bool transparency,
                PceTileId* dstId,
                vector<PcePattern>& rawTiles,
                int forcePaletteLine = -1) {
  
  // Get target graphic
//  TGraphic tileG(PcePattern::w, PcePattern::h);
//  tileG.copy(srcG,
//             TRect(0, 0, 0, 0),
//             TRect(x, y, 0, 0));
  
  int paletteNum = 0;
  PcePattern pattern;
  
  // If palette forcing is on, use the specified palette
  if (forcePaletteLine != -1) {
    paletteNum = forcePaletteLine;
    int result = pattern.fromGraphic(
                   srcG, x, y, &(palette.paletteLines[forcePaletteLine]),
                   transparency);
    if (result != 0) paletteNum = PcePalette::numPaletteLines;
  }
  else {
    // try to find a palette that matches this part of the image
    for ( ; paletteNum < PcePalette::numPaletteLines; paletteNum++) {
      // ensure no color in palette is available (which might result
      // in it changing later)
/*      bool canUse = true;
      for (int i = 0; i < PcePaletteLine::numColors; i++) {
        if (paletteUseInfo.getLineInfo(paletteNum).getAvailability(i)) {
          canUse = false;
          break;
        }
      }
      if (!canUse) continue; */
      
      int result = pattern.fromGraphic(
                     srcG, x, y, &(palette.paletteLines[paletteNum]),
                     transparency);
//      cerr << x << " " << y << " " << paletteNum << " " << result << endl;
      if (result == 0) {
        // ensure none of the colors selected is actually a free color,
        // which may change later
        bool canUse = true;
        for (int j = 0; j < PcePattern::h; j++) {
          for (int i = 0; i < PcePattern::w; i++) {
            int index = pattern.getPixel(i, j);
            if (paletteUseInfo.getLineInfo(paletteNum).getAvailability(index)) {
              canUse = false;
              break;
            }
          }
        }
/*        for (int i = 0; i < PcePaletteLine::numColors; i++) {
          if (paletteUseInfo.getLineInfo(paletteNum).getAvailability(i)) {
            canUse = false;
            break;
          }
        } */
        if (!canUse) continue;
        
        break;
      }
    }
    
    // if no palette matches, try to modify an existing one to include
    // the needed colors
    if (paletteNum >= PcePalette::numPaletteLines) {
      std::map<TColor, bool> usedColors
        = getColorsInArea(srcG, x, y, PcePattern::w, PcePattern::h);
      
      if (usedColors.size() > 16) {
        std::cerr << "Too many colors in one tile!" << std::endl;
        return -1;
      }
      
//      std::cerr << usedColors.size() << std::endl;
      for (int i = 0; i < PcePalette::numPaletteLines; i++) {
        // init "needs insertion" bool for each used color to true
        for (std::map<TColor, bool>::iterator it = usedColors.begin();
             it != usedColors.end();
             ++it) {
          it->second = true;
        }
      
        UsedPaletteLineColorSet& lineInfo = paletteUseInfo.getLineInfo(i);
        
        int freeColorsNeeded = usedColors.size();
        
        // subtract any colors already in this line from the number of free
        // colors needed
//        for (int j = 0; j < PcePaletteLine::numColors; j++) {
        for (std::map<TColor, bool>::iterator it = usedColors.begin();
             it != usedColors.end();
             ++it) {
          // FIXME: need proper transparency handling (user-selectable?)
          int checkIndex = palette.paletteLines[i].matchColor(it->first);
//          int checkIndex = palette.paletteLines[i].matchColor(
//            it->first, false);
          if (checkIndex != -1) {
            // only check colors not marked as available (i.e. filled slots)
            if (lineInfo.getAvailability(checkIndex) == false) {
              --freeColorsNeeded;
              it->second = false;
            }
          }
        }
        
        // check if number of indices remaining in this line is enough to hold
        // all needed colors
        if (lineInfo.numAvailableIndices() < freeColorsNeeded) continue;
        
        // add new colors to line
        for (std::map<TColor, bool>::iterator it = usedColors.begin();
             it != usedColors.end();
             ++it) {
//          cerr << (int)it->first.r() << " " << (int)it->first.g() << " " << (int)it->first.b() << " " << endl;
          if (it->second == false) continue;
//          std::cerr << "here" << endl;
          
          PceColor pceColor;
          pceColor.setRealColor(it->first);
//          std::cerr << hex << pceColor.getNative() << endl;
          
/*          cerr << (int)it->first.r() << " " << (int)it->first.g() << " " << (int)it->first.b() << " " << endl;
          PceColor test;
          test.setRealColor(it->first);
          cerr << (int)test.getNative() << endl;
          test.fromNative(test.getNative());
          cerr << (int)test.getNative() << endl;
          cerr << (int)test.realColor().r() << " " << (int)test.realColor().g() << " " << (int)test.realColor().b() << " " << endl;
          PceColor test2;
          test2.setRealColor(test.realColor());
          cerr << (int)test2.getNative() << endl; */
          
          int nextIndex = lineInfo.nextAvailableIndex();
          
          if (nextIndex == -1) {
            throw TGenericException(T_SRCANDLINE,
                                    "processTile()",
                                    "Impossible palette line condition");
          }
          
          lineInfo.claimIndex(nextIndex);
          palette.paletteLines[i].colors[nextIndex] = pceColor;
//          std::cerr << "index: " << i << " " << nextIndex << endl;
//          std::cerr << hex << palette.paletteLines[i].colors[nextIndex].getNative() << endl;
        }

/*    for (int i = 0; i < 16; i++) {
      cerr << "pal " << i << endl;
      for (int j = 0; j < 16; j++) {
        cerr << "  " << hex << palette.paletteLines[i].colors[j].getNative() << endl;
      }
    }
    std::cerr << i << endl; */
        
        // use this line
        paletteNum = i;
        break;
      }
      
      // check for failure
      if (paletteNum < PcePalette::numPaletteLines) {
        int result = pattern.fromGraphic(
                       srcG, x, y, &(palette.paletteLines[paletteNum]),
                       transparency);
        if (result != 0) {
          throw TGenericException(T_SRCANDLINE,
                                  "processTile()",
                                  "Impossible pattern conversion condition");
        }
      }
    }
  }
  
  if (paletteNum >= PcePalette::numPaletteLines) {
    return -1;
  }
  
//  std::cerr << paletteNum << std::endl;
  
  dstId->palette = paletteNum;
  
  // Determine if target graphic matches any existing tile.
  // If so, we don't need to add a new tile.
  bool foundMatch = false;
  for (int i = 0; i < rawTiles.size(); i++) {
    if (pattern == rawTiles[i]) {
      dstId->pattern = i;
      
      foundMatch = true;
      break;
    }
  }
  
  // if we found a match, we're done
  if (foundMatch) {
//    cout << dstId->pattern << endl;
    return 0;
  }
  
  // otherwise, add a new tile
  
  rawTiles.push_back(pattern);
  
  dstId->pattern = rawTiles.size() - 1;
  
  return 0;
}



int main(int argc, char* argv[]) {
  // Input:
  // * output filename for graphics
  //   (tilemaps assumed from input names)
  // * raw graphic(s)
  // * target offset in VRAM of tilemapped data
  // * optional output prefix
  // * palette
  //   (don't think we need this on a per-file basis?)
  
  if (argc < 2) {
    cout << "PC-Engine tilemap generator" << endl;
    cout << "Usage: " << argv[0] << " <scriptfile>" << endl;
    
    return 0;
  }
  
  TIniFile script = TIniFile(string(argv[1]));
  
  if (!script.hasSection("Properties")) {
    cerr << "Error: Script has no 'Properties' section" << endl;
    return 1;
  }
  
  string paletteName, destName;
  int loadAddr = 0;
  int minTiles = 0;
  int maxTiles = -1;
  Blacklist blacklist;
  
  // Mandatory fields
  
  if (!script.hasKey("Properties", "palette")) {
    cerr << "Error: Properties.palette is undefined" << endl;
    return 1;
  }
  paletteName = script.valueOfKey("Properties", "palette");
  
  if (!script.hasKey("Properties", "dest")) {
    cerr << "Error: Properties.dest is undefined" << endl;
    return 1;
  }
  destName = script.valueOfKey("Properties", "dest");
  
  // Optional fields
  
  if (script.hasKey("Properties", "loadAddr")) {
    loadAddr = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "loadAddr"));
  }
  
  if (script.hasKey("Properties", "minTiles")) {
    minTiles = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "minTiles"));
  }
  
  if (script.hasKey("Properties", "maxTiles")) {
    maxTiles = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "maxTiles"));
  }
  
  if (script.hasKey("Properties", "blacklist")) {
    string blacklistStr = script.valueOfKey("Properties", "blacklist");
    readBlacklist(blacklist, blacklistStr);
  }
  
  if (script.hasKey("Properties", "freepalettes")) {
    string freepalettesStr = script.valueOfKey("Properties", "freepalettes");
    paletteUseInfo.readSpecifierString(freepalettesStr);
  }
  
//  for (int i = 0; i < blacklist.size(); i++) {
//    cout << hex << blacklist[i] << endl;
//  }
  
  // get palette
  PcePalette palette;
  {
//    TArray<TByte> rawPalette;
//    TFileManip::readEntireFile(rawPalette, paletteName);
    TBufStream ifs;
    ifs.open(paletteName.c_str());
    if (ifs.size() <= 0) {
      cerr << "Error: Palette " << paletteName
        << " does not exist or couldn't be opened" << endl;
      return 1;
    }
    palette.read(ifs);
/*    cout << std::hex << (int)palette.paletteLines[0].colors[0].realColor().r()
         << " "
         << std::hex << (int)palette.paletteLines[0].colors[0].realColor().g()
         << " "
         << std::hex << (int)palette.paletteLines[0].colors[0].realColor().b()
         << endl; */
    
//    TBufStream ofs;
//    palette.write(ofs);
//    ofs.save("debug.bin");
  }
  
  // 1. go through all source images and analyze for matching tiles
  // 2. create per-image tilemap corresponding to raw tile indices
  // 3. map raw tile indices to actual tile positions (accounting for
  //    blacklist, etc.)
  // 4. generate final tilemaps by mapping raw indices to final positions
  
//  vector<PceTilemap> rawTilemaps;
  map<string, PceTilemap> rawTilemaps;
  vector<PcePattern> rawTiles;
  
  for (SectionKeysMap::const_iterator it = script.cbegin();
       it != script.cend();
       ++it) {
    // iterate over all sections beginning with "Tilemap"
    string cmpstr = "Tilemap";
    if (it->first.substr(0, cmpstr.size()).compare(cmpstr) != 0) continue;
    string sectionName = it->first;
    
    string sourceStr;
    int forcePaletteLine = -1;
    
    // mandatory fields
    
    if (!script.hasKey(sectionName, "source")) {
      cerr << "Error: " << sectionName << ".source is undefined" << endl;
      return 1;
    }
    sourceStr = script.valueOfKey(sectionName, "source");
    
    std::cerr << "processing: " << sourceStr << endl;
    
    // optional fields
    
    if (script.hasKey(sectionName, "palette")) {
      forcePaletteLine = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "palette"));
    }
    
    // FIXME: will not work with blacklist
    if (script.hasKey(sectionName, "forceTilePrePadding")) {
      int prePaddingSize = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "forceTilePrePadding"));
      while (rawTiles.size() < prePaddingSize)
        rawTiles.push_back(PcePattern());
    }
    
    // get source graphic
    TGraphic srcG;
    TPngConversion::RGBAPngToGraphic(sourceStr, srcG);
//    TPngConversion::graphicToRGBAPng(std::string("debug.png"), srcG);
    
    // infer tilemap dimensions from source size
    int tileW = srcG.w() / PcePattern::w;
    int tileH = srcG.h() / PcePattern::h;
    
    PceTilemap tilemap;
    tilemap.resize(tileW, tileH);
    
//    cout << tileW << " " << tileH << endl;

    bool transparency = true;
    
    if (script.hasKey(sectionName, "transparency")) {
      transparency = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "transparency")) != 0;
    }

    for (int j = 0; j < tilemap.tileIds.h(); j++) {
      for (int i = 0; i < tilemap.tileIds.w(); i++) {
//        std::cerr << "pos: " << i << ", " << j << std::endl;
        PceTileId& tileId = tilemap.tileIds.data(i, j);
      
        int result = processTile(srcG, i * PcePattern::w, j * PcePattern::h,
                                 palette,
                                 transparency,
                                 &tileId,
                                 rawTiles,
                                 forcePaletteLine);
        
        if (result != 0) {
          cerr << "Error in " << sectionName
            << ": failed processing tile (" << i << ", " << j << ")" << endl;
          return 2;
        }
      }
    }
    
    rawTilemaps[sectionName] = tilemap;
  }
  
//  cout << rawTiles.size() << endl;

  // Produce the final arrangement of tiles
  
  map<int, PcePattern> outputTiles;
  map<int, int> rawToOutputMap;
  int outputTileNum = 0;
  for (int i = 0; i < rawTiles.size(); i++) {
//    std::cerr << std::hex << outputTileNum + loadAddr << std::endl;
    // Skip blacklisted content
    while (isBlacklisted(blacklist, outputTileNum + loadAddr)) {
      outputTiles[outputTileNum] = PcePattern();
      ++outputTileNum;
    }
    
    outputTiles[outputTileNum] = rawTiles[i];
    rawToOutputMap[i] = outputTileNum;
    ++outputTileNum;
  }
  
  // Give an error if tile limit exceeded
  if (outputTiles.size() > maxTiles) {
    cerr << "Error: Tile limit exceeded (limit is "
      << maxTiles << "; generated "
      << outputTiles.size() << ")" << endl;
    return -3;
  }
  
  // Write tile data
  {
    TOfstream ofs(destName.c_str(), ios_base::binary);
    for (map<int, PcePattern>::const_iterator it = outputTiles.cbegin();
         it != outputTiles.cend();
         ++it) {
      it->second.write(ofs);
    }
    
    // pad with extra tiles to meet minimum length
    int padTiles = minTiles - outputTiles.size();
    PcePattern pattern;
    for (int i = 0; i < padTiles; i++) {
      pattern.write(ofs);
    }
  }
  
  // Update tilemaps and write to destinations
  
  for (SectionKeysMap::const_iterator it = script.cbegin();
       it != script.cend();
       ++it) {
    // iterate over all sections beginning with "Tilemap"
    string cmpstr = "Tilemap";
    if (it->first.substr(0, cmpstr.size()).compare(cmpstr) != 0) continue;
    string sectionName = it->first;
    
    if (!script.hasKey(sectionName, "dest")) {
      cerr << "Error: " << sectionName << ".dest is undefined" << endl;
      return 1;
    }
    string destStr = script.valueOfKey(sectionName, "dest");
    
    PceTilemap& tilemap = rawTilemaps[sectionName];
    
    TOfstream ofs(destStr.c_str(), ios_base::binary);
    
    for (int j = 0; j < tilemap.tileIds.h(); j++) {
      for (int i = 0; i < tilemap.tileIds.w(); i++) {
        PceTileId& id = tilemap.tileIds.data(i, j);
        id.pattern = rawToOutputMap[id.pattern] + loadAddr;
        char buffer[PceTileId::size];
        id.write(buffer);
        ofs.write(buffer, PceTileId::size);
      }
    }
    
  }
  
  // save palette if requested
  if (script.hasKey("Properties", "paldest")) {
    std::string paldest = script.valueOfKey("Properties", "paldest");
    TBufStream ofs;
    palette.write(ofs);
    ofs.save(paldest.c_str());
  }
  
  return 0;
}
