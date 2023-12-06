#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "util/TParse.h"
#include "pce/PceSpritePattern.h"
#include "pce/PceSpriteId.h"
#include "pce/PcePalette.h"
#include "pce/PcePaletteLine.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace BlackT;
using namespace Pce;

//const static int patternsPerRow = 16;

class ExtPceSpriteId : public PceSpriteId {
public:
  enum Cmd {
    cmd_normal,
    cmd_output,
    cmd_set,
    cmd_outputPreviewGrp
  };
  
  ExtPceSpriteId()
    : PceSpriteId(),
      xOffset(0),
      yOffset(0),
      xRepeat(1),
      yRepeat(1),
      cmd(cmd_normal),
      setValue(0) { }
  
  int xOffset;
  int yOffset;
  int xRepeat;
  int yRepeat;
  Cmd cmd;
  
  std::string outputName;
  std::string setName;
  int setValue;
  
protected:
  
};

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "PC Engine-format sprite builder" << endl;
    cout << "Usage: " << argv[0] << " <inpng> <inscript> <inpal> <outgrp> [options]" << endl;
    
/*    cout << "Options: " << endl;
    cout << "  -basepattern   " << "Sets base pattern number" << endl;
    cout << "  -xoffset       " << "X-offset applies to output" << endl;
    cout << "  -yoffset       " << "Y-offset applies to output" << endl;*/
    
    return 0;
  }
  
  string inPng = string(argv[1]);
  string inScript = string(argv[2]);
  string inPal = string(argv[3]);
  string outGrp = string(argv[4]);
//  string outSpr = string(argv[5]);
  
  int basePattern = 0;
//  TOpt::readNumericOpt(argc, argv, "-basepattern", &basePattern);
  
  int xOffset = 0;
  int xOrigin = 0;
//  TOpt::readNumericOpt(argc, argv, "-xoffset", &xOffset);
  
  int yOffset = 0;
  int yOrigin = 0;
//  TOpt::readNumericOpt(argc, argv, "-yoffset", &yOffset);

  int patternOffset = 0;
  int paletteOffset = 0;

  int padSize = 0;
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(inPng, grp);
  
  PcePalette palette;
  {
    TBufStream ifs;
    ifs.open(inPal.c_str());
    palette.read(ifs);
  }
  
  TBufStream scriptIfs;
  scriptIfs.open(inScript.c_str());
  
  TBufStream grpOfs;
  
  vector<ExtPceSpriteId> sprites;
  
  //=============================
  // parse script
  //=============================
  
  int lineNum = 0;
//  std::cerr << inScript << std::endl;
//  std::cerr << "------------------------" << std::endl;
  while (!scriptIfs.eof()) {
    std::string line;
    scriptIfs.getLine(line);
    ++lineNum;
    
    if (line.size() <= 0) continue;
    
    TBufStream ifs;
    ifs.writeString(line);
    ifs.seek(0);
    
    TParse::skipSpace(ifs);
    if (ifs.eof()) continue;
    if (ifs.peek() == '#') continue;
    
    std::string cmd = TParse::getSpacedSymbol(ifs);
    if (cmd.compare("add") == 0) {
//      std::cerr << "adding: " << std::endl;
      ExtPceSpriteId sprite;
    
      TParse::skipSpace(ifs);
      while (!ifs.eof()) {
        TParse::skipSpace(ifs);
        
        std::string subCmd = TParse::getUntilChars(ifs, "=");
//        TParse::matchChar(ifs, '=');
        int param = TParse::matchInt(ifs);
        
        if (subCmd.compare("x") == 0) sprite.x = param;
        else if (subCmd.compare("y") == 0) sprite.y = param;
        else if (subCmd.compare("xoff") == 0) sprite.xOffset = param;
        else if (subCmd.compare("yoff") == 0) sprite.yOffset = param;
        else if (subCmd.compare("xrept") == 0) sprite.xRepeat = param;
        else if (subCmd.compare("yrept") == 0) sprite.yRepeat = param;
        else if (subCmd.compare("w") == 0)
          sprite.width = (param / PceSpritePattern::w) - 1;
        else if (subCmd.compare("h") == 0)
          sprite.height = (param / PceSpritePattern::h) - 1;
        else if (subCmd.compare("pal") == 0) sprite.palette = param;
        else if (subCmd.compare("pri") == 0) sprite.priority = (param != 0);
        else {
          std::cerr << "Line " << lineNum << ": unknown add subcommand '"
            << subCmd << "'" << std::endl;
          return 1;
        }
      }
      
//      std::cerr << sprite.x << " " << sprite.y << std::endl;
      sprites.push_back(sprite);
    }
    else if (cmd.compare("set") == 0) {
      TParse::skipSpace(ifs);
      while (!ifs.eof()) {
        TParse::skipSpace(ifs);
        
        std::string subCmd = TParse::getUntilChars(ifs, "=");
        int param = TParse::matchInt(ifs);
        
        ExtPceSpriteId sprite;
        sprite.cmd = ExtPceSpriteId::cmd_set;
        sprite.setName = subCmd;
        sprite.setValue = param;
        sprites.push_back(sprite);
        
/*        if (subCmd.compare("basepattern") == 0) basePattern = param;
        else if (subCmd.compare("xoffset") == 0) xOffset = param;
        else if (subCmd.compare("yoffset") == 0) yOffset = param;
        else if (subCmd.compare("padsize") == 0) padSize = param;
        else {
          std::cerr << "Line " << lineNum << ": unknown set subcommand '"
            << subCmd << "'" << std::endl;
          return 1;
        }*/
      }
    }
    else if (cmd.compare("outputSprites") == 0) {
      ExtPceSpriteId sprite;
      sprite.cmd = ExtPceSpriteId::cmd_output;
      sprite.outputName = TParse::matchString(ifs);
      sprites.push_back(sprite);
    }
    else if (cmd.compare("outputPreviewGrp") == 0) {
      ExtPceSpriteId sprite;
      sprite.cmd = ExtPceSpriteId::cmd_outputPreviewGrp;
      sprite.outputName = TParse::matchString(ifs);
      sprites.push_back(sprite);
    }
    else {
      std::cerr << "Line " << lineNum << ": unknown command '"
        << cmd << "'" << std::endl;
      return 1;
    }
  }
  
  //=============================
  // read sprites
  //=============================
  
  // HACK: write sprite count to start of list
//  sprOfs.writeu8(sprites.size());
//  sprOfs.writeu8(0x00);
  
  TGraphic previewGrp = grp;
  previewGrp.clear(TColor(0, 0, 0));
  
  int currentPatternNum = 0;
  TBufStream sprOfs;
  for (vector<ExtPceSpriteId>::iterator it = sprites.begin();
       it != sprites.end();
       ++it) {
    if (it->cmd == ExtPceSpriteId::cmd_output) {
/*      // pad to specified output size
      // TODO
      if (sprites.size() < padSize) {
        int remaining = padSize - sprites.size();
        for (int i = 0; i < remaining; i++) {
          for (int j = 0; j < ExtPceSpriteId::size; j++) {
            sprOfs.put(0x00);
          }
        }
      }*/
      
      sprOfs.save(it->outputName.c_str());
      sprOfs = TBufStream();
      continue;
    }
    else if (it->cmd == ExtPceSpriteId::cmd_set) {
      std::string subCmd = it->setName;
      int param = it->setValue;
      
      if (subCmd.compare("basepattern") == 0) basePattern = param;
      else if (subCmd.compare("xoffset") == 0) xOffset = param;
      else if (subCmd.compare("yoffset") == 0) yOffset = param;
      else if (subCmd.compare("xorigin") == 0) xOrigin = param;
      else if (subCmd.compare("yorigin") == 0) yOrigin = param;
      else if (subCmd.compare("patternoffset") == 0) patternOffset = param;
      else if (subCmd.compare("paletteoffset") == 0) paletteOffset = param;
      else if (subCmd.compare("padsize") == 0) padSize = param;
      else {
        std::cerr << "Line " << lineNum << ": unknown set subcommand '"
          << subCmd << "'" << std::endl;
        return 1;
      }
      
      continue;
    }
    else if (it->cmd == ExtPceSpriteId::cmd_outputPreviewGrp) {
      TPngConversion::graphicToRGBAPng(it->outputName, previewGrp);
      previewGrp = TGraphic();
      continue;
    }
    
    int x = it->x;
    int y = it->y;
    int w = it->width + 1;
    int h = it->height + 1;
    int palIndex = it->palette;
    
    // adjust output pattern according to base
    it->pattern = basePattern + currentPatternNum;
    
    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        int realX = x + (i * PceSpritePattern::w);
        int realY = y + (j * PceSpritePattern::h);
        
        PceSpritePattern spritePattern;
        int result = spritePattern.fromGraphic(grp, realX, realY,
                                    &palette.paletteLines[palIndex]);
        if (result != 0) {
          cerr << "Error processing subsprite at ("
            << x << ", " << y << ")"
            << ", true coords (" << realX << ", " << realY << ")"
            << ": cannot convert using palette line"
            << palIndex << endl;
          return 1;
        }
        
        spritePattern.write(grpOfs);
        ++currentPatternNum;
      }
    }
    
//    previewGrp.fillRect(x, y, w * PceSpritePattern::w, h * PceSpritePattern::h,
//                        TColor(255, 255, 255, TColor::fullAlphaTransparency));
    for (int j = 0; j < h * PceSpritePattern::h; j++) {
      for (int i = 0; i < w * PceSpritePattern::w; i++) {
        TColor color = previewGrp.getPixel(x + i, y + j);
        color.setR(color.r() + 64);
        previewGrp.setPixel(x + i, y + j, color);
      }
    }
    
    // adjust output position according to offset
    it->x = x - xOrigin + xOffset + it->xOffset;
    it->y = y - yOrigin + yOffset + it->yOffset;
    
    // adjust tile num according to offset
    it->pattern += patternOffset;
    it->palette += paletteOffset;
    
//    it->write(sprOfs);
    // FIXME: assumes 16x16 sprites for now
    for (int j = 0; j < it->yRepeat; j++) {
      for (int i = 0; i < it->xRepeat; i++) {
        ExtPceSpriteId temp = *it;
        temp.x += (i * PceSpritePattern::w);
        temp.y += (j * PceSpritePattern::h);
        temp.write(sprOfs);
      }
    }
    
    // HACK: ???
/*    sprOfs.seekoff(-4);
    int raw = sprOfs.readu16le();
//    raw |= 0x0200;
//    raw = ((raw >> 1) + 0x0100) << 1;
    raw += 0x200;
    sprOfs.seekoff(-2);
    sprOfs.writeu16le(raw);
    sprOfs.seekoff(2);*/
  }
  
  // write finished size
/*  {
    int pos = sprOfs.tell();
    sprOfs.seek(0);
    sprOfs.writeu8((sprOfs.size() - 1) / ExtPceSpriteId::size);
    sprOfs.seek(pos);
  }*/
  
  //=============================
  // save output
  //=============================
  
  grpOfs.save(outGrp.c_str());
//  sprOfs.save(outSpr.c_str());
  
//  TPngConversion::graphicToRGBAPng("test.png", previewGrp);
  
  return 0;
}
