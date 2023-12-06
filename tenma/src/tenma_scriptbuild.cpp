#include "tenma/TenmaScriptReader.h"
#include "tenma/TenmaLineWrapper.h"
#include "tenma/TenmaMsgConsts.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TPngConversion.h"
#include "util/TFreeSpace.h"
#include "util/TBitmapFont.h"
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

// generating the compression dictionary is time-consuming once the
// input gets large enough, and rather pointless since it will essentially
// never vary for a large input size.
// if set to 0, running this program will not generate the compression
// dictionary; instead, it will simply use a pre-built dictionary read from
// the same location that it normally writes the generated dictionary to.
// (though in practice the time seems to be dominated by applying the
// compression, not building it, though that may just mean I did a
// particularly bad job of implementing it)
#define BUILD_COMPRESSION_DICTIONARY 1

using namespace std;
using namespace BlackT;
using namespace Pce;

const static int sectorSize = 0x800;

const static int textCharsStart = 0x00 + 8;
const static int textCharsEnd = textCharsStart + 0x60;
const static int textEncodingMax = 0xE0;
const static int maxDictionarySymbols = textEncodingMax - textCharsEnd;

// TODO
const static int fontEmphToggleOp = 0x5F + 8;

// jump printing to a new source (within the current memory map)
const static int code_localStringRedirect = 0xEE;
// copy a string from a location in a reserved memory bank to a fixed
// buffer in the kernel's memory space, then jump to it as with a
// local redirect.
// this is not designed to work universally; it's intended for
// printing item names, which have been moved out of the kernel
// to free up memory there.
const static int code_globalStringRedirect = 0xEA;

TThingyTable tableStd;
TThingyTable tableMenu;
TThingyTable tableScene;

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexByteNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  return string("$") + str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}
                      

void binToDcb(TStream& ifs, std::ostream& ofs) {
  int constsPerLine = 16;
  
  while (true) {
    if (ifs.eof()) break;
    
    ofs << "  .db ";
    
    for (int i = 0; i < constsPerLine; i++) {
      if (ifs.eof()) break;
      
      TByte next = ifs.get();
      ofs << as2bHexPrefix(next);
      if (!ifs.eof() && (i != constsPerLine - 1)) ofs << ",";
    }
    
    ofs << std::endl;
  }
}




typedef std::map<std::string, int> UseCountTable;
//typedef std::map<std::string, double> EfficiencyTable;
typedef std::map<double, std::string> EfficiencyTable;

bool isCompressible(std::string& str) {
  for (int i = 0; i < str.size(); i++) {
    if ((unsigned char)str[i] < textCharsStart) return false;
    if ((unsigned char)str[i] >= textCharsEnd) return false;
    if ((unsigned char)str[i] == fontEmphToggleOp) return false;
  }
  
  return true;
}

void addStringToUseCountTable(std::string& input,
                        UseCountTable& useCountTable,
                        int minLength, int maxLength) {
  int total = input.size() - minLength;
  if (total <= 0) return;
  
  for (int i = 0; i < total; ) {
    int basePos = i;
    for (int j = minLength; j < maxLength; j++) {
      int length = j;
      if (basePos + length >= input.size()) break;
      
      std::string str = input.substr(basePos, length);
      
      // HACK: avoid analyzing parameters of control sequences
      // the ops themselves are already ignored in the isCompressible check;
      // we just check when an op enters into the first byte of the string,
      // then advance the check position so the parameter byte will
      // never be considered
/*      if ((str.size() > 0) && ((unsigned char)str[0] < textCharsStart)) {
        unsigned char value = str[0];
        if ((value == 0x02) // "L"
            || (value == 0x05) // "P"
            || (value == 0x06)) { // "W"
          // skip the argument byte
          i += 1;
        }
        break;
      }*/
      if (str.size() > 0) {
        unsigned char value = str[0];
        if (TenmaMsgConsts::isOp(value)) {
          // skip the arguments
          i += TenmaMsgConsts::getOpArgsSize(value);
          break;
        }
      }
      
      if (!isCompressible(str)) break;
      
      ++(useCountTable[str]);
    }
    
    // skip literal arguments to ops
/*    if ((unsigned char)input[i] < textCharsStart) {
      ++i;
      int opSize = numOpParamWords((unsigned char)input[i]);
      i += opSize;
    }
    else {
      ++i;
    } */
    ++i;
  }
}

void addRegionsToUseCountTable(TenmaScriptReader::NameToRegionMap& input,
                        UseCountTable& useCountTable,
                        int minLength, int maxLength) {
  for (TenmaScriptReader::NameToRegionMap::iterator it = input.begin();
       it != input.end();
       ++it) {
    TenmaScriptReader::ResultCollection& results = it->second.strings;
    for (TenmaScriptReader::ResultCollection::iterator jt = results.begin();
         jt != results.end();
         ++jt) {
//      std::cerr << jt->srcOffset << std::endl;
      if (jt->isLiteral) continue;
      if (jt->isNotCompressible) continue;
      
      addStringToUseCountTable(jt->str, useCountTable,
                               minLength, maxLength);
    }
  }
}

void buildEfficiencyTable(UseCountTable& useCountTable,
                        EfficiencyTable& efficiencyTable) {
  for (UseCountTable::iterator it = useCountTable.begin();
       it != useCountTable.end();
       ++it) {
    std::string str = it->first;
    // penalize by 1 byte (length of the dictionary code)
    double strLen = str.size() - 1;
    double uses = it->second;
//    efficiencyTable[str] = strLen / uses;
    
    efficiencyTable[strLen / uses] = str;
  }
}

void applyDictionaryEntry(std::string entry,
                          TenmaScriptReader::NameToRegionMap& input,
                          std::string replacement) {
  for (TenmaScriptReader::NameToRegionMap::iterator it = input.begin();
       it != input.end();
       ++it) {
    TenmaScriptReader::ResultCollection& results = it->second.strings;
    int index = -1;
    for (TenmaScriptReader::ResultCollection::iterator jt = results.begin();
         jt != results.end();
         ++jt) {
      ++index;
      
      if (jt->isNotCompressible) continue;
      
      std::string str = jt->str;
      if (str.size() < entry.size()) continue;
      
      std::string newStr;
      int i;
      for (i = 0; i < str.size() - entry.size(); ) {
        if (TenmaMsgConsts::isOp((unsigned char)str[i])) {
/*          int numParams = numOpParamWords((unsigned char)str[i]);
          
          newStr += str[i];
          for (int j = 0; j < numParams; j++) {
            newStr += str[i + 1 + j];
          }
          
          ++i;
          i += numParams; */
          
/*          newStr += str[i];
          ++i;
          continue;*/
          
/*          if (jt->id.compare("area-0x1F3E-0x7FBC") == 0) {
            std::cerr << "here" << std::endl;
            std::cerr << "op: " << std::hex << (unsigned char)str[i] << std::endl;
            std::cerr << "start:  " << std::hex << i << std::endl;
          }*/
          
          int numParams = TenmaMsgConsts::getOpArgsSize((unsigned char)str[i]);
          newStr += str[i++];
          for (int j = 0; j < numParams; j++) {
            newStr += str[i + j];
          }
          i += numParams;
          
/*          if (jt->id.compare("area-0x1F3E-0x7FBC") == 0) {
            std::cerr << "size: " << std::dec << numParams << std::endl;
            std::cerr << "finish: " << std::hex << i << std::endl;
            char c;
            std::cin >> c;
          }*/
          
          continue;
        }
        
        if (entry.compare(str.substr(i, entry.size())) == 0) {
          newStr += replacement;
          i += entry.size();
        }
        else {
          newStr += str[i];
          ++i;
        }
      }
      
      while (i < str.size()) newStr += str[i++];
      
      jt->str = newStr;
    }
  }
}

void generateCompressionDictionary(
    TenmaScriptReader::NameToRegionMap& results,
    std::string outputDictFileName) {
  TBufStream dictOfs;
  for (int i = 0; i < maxDictionarySymbols; i++) {
//    cerr << i << endl;
    UseCountTable useCountTable;
    addRegionsToUseCountTable(results, useCountTable, 2, 3);
    EfficiencyTable efficiencyTable;
    buildEfficiencyTable(useCountTable, efficiencyTable);
    
//    std::cout << efficiencyTable.begin()->first << std::endl;
    
    // if no compressions are possible, give up
    if (efficiencyTable.empty()) break;
    
    int symbol = i + textCharsEnd;
    applyDictionaryEntry(efficiencyTable.begin()->second,
                         results,
                         std::string() + (char)symbol);
    
    // debug
/*    TBufStream temp;
    temp.writeString(efficiencyTable.begin()->second);
    temp.seek(0);
//    binToDcb(temp, cout);
    std::cout << "\"";
    while (!temp.eof()) {
      std::cout << table.getEntry(temp.get());
    }
    std::cout << "\"" << std::endl; */
    
    dictOfs.writeString(efficiencyTable.begin()->second);
  }
  
//  dictOfs.save((outPrefix + "dictionary.bin").c_str());
  dictOfs.save(outputDictFileName.c_str());
}

// merge a set of NameToRegionMaps into a single NameToRegionMap
void mergeResultMaps(
    std::vector<TenmaScriptReader::NameToRegionMap*>& allSrcPtrs,
    TenmaScriptReader::NameToRegionMap& dst) {
  int targetOutputId = 0;
  for (std::vector<TenmaScriptReader::NameToRegionMap*>::iterator it
        = allSrcPtrs.begin();
       it != allSrcPtrs.end();
       ++it) {
    TenmaScriptReader::NameToRegionMap& src = **it;
    for (TenmaScriptReader::NameToRegionMap::iterator jt = src.begin();
         jt != src.end();
         ++jt) {
      dst[TStringConversion::intToString(targetOutputId++)] = jt->second;
    }
  }
}

// undo the effect of mergeResultMaps(), applying any changes made to
// the merged maps back to the separate originals
void unmergeResultMaps(
    TenmaScriptReader::NameToRegionMap& src,
    std::vector<TenmaScriptReader::NameToRegionMap*>& allSrcPtrs) {
  int targetInputId = 0;
  for (std::vector<TenmaScriptReader::NameToRegionMap*>::iterator it
        = allSrcPtrs.begin();
       it != allSrcPtrs.end();
       ++it) {
    TenmaScriptReader::NameToRegionMap& dst = **it;
    for (TenmaScriptReader::NameToRegionMap::iterator jt = dst.begin();
         jt != dst.end();
         ++jt) {
      jt->second = src[TStringConversion::intToString(targetInputId++)];
    }
  }
}

void exportGenericString(TenmaScriptReader::ResultString& str,
                         std::string prefix) {
  if (str.str.size() <= 0) return;
  
  std::string outName = prefix + str.id + ".bin";
  TFileManip::createDirectoryForFile(outName);
  
  TBufStream ofs;
  ofs.writeString(str.str);
  
  if (str.propertyIsTrue("terminatedByPrompt")
        || str.propertyIsTrue("terminatedByF9")) {
  
  }
  else if (!str.propertyIsTrue("noTerminator")) {
    ofs.writeu8(TenmaMsgConsts::opcode_end);
  }
  
  ofs.save(outName.c_str());
}

void exportGenericRegion(TenmaScriptReader::ResultCollection& results,
                         std::string prefix) {
  for (TenmaScriptReader::ResultCollection::iterator it = results.begin();
       it != results.end();
       ++it) {
//    if (it->str.size() <= 0) continue;
    TenmaScriptReader::ResultString& str = *it;
    exportGenericString(str, prefix);
  }
}

void exportGenericRegionMap(TenmaScriptReader::NameToRegionMap& results,
                         std::string prefix) {
  for (auto it: results) {
    exportGenericRegion(it.second.strings, prefix);
  }
}

void exportRegionAdvSceneInclude(TenmaScriptReader::ResultRegion& results,
                         std::string regionName, std::string prefix) {
  std::string fileName = prefix + regionName + ".inc";
  TFileManip::createDirectoryForFile(fileName);
  std::ofstream ofs(fileName.c_str());
  
  for (auto it: results.freeSpace.freeSpace_) {
    // size is reduced by 4 to leave space for end-of-bank content marker
    // and script start pointer
    ofs << ".unbackground " << it.first << " " << it.first + it.second - 1 - 4
      << std::endl;
  }
  
//  ofs << endl;
//  ofs << ".bank 0 slot 0" << endl;
//  ofs << "" << endl;
}

void exportAdvSceneIncludes(TenmaScriptReader::NameToRegionMap& results,
                         std::string prefix) {
  for (auto it: results) {
    exportRegionAdvSceneInclude(it.second, it.first, prefix);
  }
}

void exportRegionVisualInclude(TenmaScriptReader::ResultRegion& results,
                         std::string regionName, std::string prefix) {
  std::string fileName = prefix + regionName + ".inc";
  TFileManip::createDirectoryForFile(fileName);
  std::ofstream ofs(fileName.c_str());
  
  for (auto it: results.freeSpace.freeSpace_) {
    ofs << ".unbackground " << it.first << " " << it.first + it.second - 1
      << std::endl;
  }
  
  if (results.hasProperty("genSpriteTable_raw")) {
    ofs << ".define genSpriteTable $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("genSpriteTable_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("playCdTrack_raw")) {
    ofs << ".define playCdTrack $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("playCdTrack_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("cplayAdpcm_raw")) {
    ofs << ".define cplayAdpcm $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("cplayAdpcm_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("setSceneTimer_raw")) {
    ofs << ".define setSceneTimer $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("setSceneTimer_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("currentSpriteCount_ptr")) {
    ofs << ".define currentSpriteCount $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("currentSpriteCount_ptr"))
      << endl;
  }
  
  if (results.hasProperty("doObjTimerSet1_raw")) {
    ofs << ".define doObjTimerSet1 $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("doObjTimerSet1_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("doObjTimerSet2_raw")) {
    ofs << ".define doObjTimerSet2 $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("doObjTimerSet2_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("sceneVramCopy_raw")) {
    ofs << ".define sceneVramCopy $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("sceneVramCopy_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("sceneTilemapCopy_raw")) {
    ofs << ".define sceneTilemapCopy $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("sceneTilemapCopy_raw"))
         + 0x4000
      << endl;
  }
  
  if (results.hasProperty("fadeArrayBase")) {
    ofs << ".define fadeArrayBase $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("fadeArrayBase"))
      << endl;
  }
  
  if (results.hasProperty("fadeOn")) {
    ofs << ".define fadeOn $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("fadeOn"))
      << endl;
  }
  
  if (results.hasProperty("fadeBgBase")) {
    ofs << ".define fadeBgBase $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("fadeBgBase"))
      << endl;
  }
  
  if (results.hasProperty("fadeSpriteBase")) {
    ofs << ".define fadeSpriteBase $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("fadeSpriteBase"))
      << endl;
  }
  
  if (results.hasProperty("readControllers_raw")) {
    ofs << ".define readControllers $" << hex
      << TStringConversion::stringToInt(
          results.properties.at("readControllers_raw"))
         + 0x4000
      << endl;
  }
}

void exportVisualIncludes(TenmaScriptReader::NameToRegionMap& results,
                         std::string prefix) {
  for (auto it: results) {
    exportRegionVisualInclude(it.second, it.first, prefix);
  }
}

void exportRegionCreditsInclude(TenmaScriptReader::ResultRegion& results,
                         std::string regionName, std::string prefix) {
  std::string fileName = prefix + "credits_text.inc";
  TFileManip::createDirectoryForFile(fileName);
  std::ofstream ofs(fileName.c_str());
  
  for (auto it: results.freeSpace.freeSpace_) {
    int start = 0x10000 + it.first + 0x6000;
    int end = start + it.second + 0x6000 - 1;
    ofs << ".unbackground " << start << " " << end
      << std::endl;
  }
  
  for (auto& it: results.strings) {
    std::string labelName = it.id;
    std::string sectionName = "credits str "
      + it.id;
    
    ofs << ".bank 1 slot 0" << endl;
    ofs << ".section \"" << sectionName << "\" free" << endl;
    ofs << "  " << labelName << ":" << endl;
//    ofs << "  .incbin \"out/script/strings/" << it.id << ".bin\"" << endl;
//    ofs << binToDcb(it.str) << endl;
    {
      TBufStream ifs;
      ifs.writeString(it.str);
      ifs.put(0x00);
      ifs.seek(0);
      binToDcb(ifs, ofs);
    }
    ofs << ".ends" << endl;
    
    for (auto& jt: it.pointerRefs) {
      ofs << ".bank 2 slot 0" << endl;
      ofs << ".orga $"
        << TStringConversion::intToString(jt + 0xC000,
              TStringConversion::baseHex).substr(2, string::npos)
        << endl;
      ofs << ".section \"" << sectionName << " ref " << jt << "\" overwrite" << endl;
      ofs << "  .dw " << labelName << endl;
      ofs << ".ends" << endl;
    }
  }
}

void exportCreditsIncludes(TenmaScriptReader::NameToRegionMap& results,
                         std::string prefix) {
  for (auto& it: results) {
    exportRegionCreditsInclude(it.second, it.first, prefix);
  }
}

void exportRegionBackUtilInclude(TenmaScriptReader::ResultRegion& results,
                         std::string regionName, std::string prefix) {
  std::string fileName = prefix + "backutil.inc";
  TFileManip::createDirectoryForFile(fileName);
  std::ofstream ofs(fileName.c_str());
  
/*  for (auto it: results.freeSpace.freeSpace_) {
    int start = 0x10000 + it.first + 0x6000;
    int end = start + it.second + 0x6000 - 1;
    ofs << ".unbackground " << start << " " << end
      << std::endl;
  }*/
  
  for (auto& it: results.strings) {
    std::string labelName = it.id;
    std::string sectionName = "backutil str "
      + it.id;
    
    // HACK
    int origOffset = -1;
    for (int i = 0; i < labelName.size() - 1; i++) {
      if ((labelName[i] == '0') && (labelName[i + 1] == 'x')) {
        origOffset = TStringConversion::stringToInt(
          labelName.substr(i, std::string::npos));
        origOffset -= 0x4000;
        break;
      }
    }
    
    ofs << ".bank bank2 slot 3" << endl;
    ofs << ".section \"" << sectionName << "\" free" << endl;
    ofs << "  " << labelName << ":" << endl;
//    ofs << "  .incbin \"out/script/strings/" << it.id << ".bin\"" << endl;
//    ofs << binToDcb(it.str) << endl;
    {
      TBufStream ifs;
      ifs.writeString(it.str);
      ifs.put(0x00);
      ifs.seek(0);
      binToDcb(ifs, ofs);
    }
    ofs << ".ends" << endl;
    
    ofs << ".bank bank2 slot 3" << endl;
    ofs << ".orga $"
      << TStringConversion::intToString(origOffset + 0xC000,
            TStringConversion::baseHex).substr(2, string::npos)
      << endl;
    ofs << ".section \"" << sectionName << " ref " << origOffset << "\" overwrite" << endl;
    ofs << "  .dw " << labelName << endl;
    ofs << ".ends" << endl;
  }
}

void exportBackUtilIncludes(TenmaScriptReader::NameToRegionMap& results,
                         std::string prefix) {
  for (auto& it: results) {
    exportRegionBackUtilInclude(it.second, it.first, prefix);
  }
}

// original table
/*const int newMenuColLayoutTable[] = {
  // 1-column menu
  0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // 2-column menu
  0x00, 0x0E, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
  // 3-column menu
  0x00, 0x08, 0x11, 0x1C, 0x00, 0x00, 0x00, 0x00,
  // 4-column menu
  0x00, 0x07, 0x0E, 0x15, 0x1C, 0x00, 0x00, 0x00,
  // 5-column menu
  0x00, 0x05, 0x0B, 0x10, 0x17, 0x1C, 0x00, 0x00,
};
const int numNewMenuColLayoutTableEntries
  = sizeof(newMenuColLayoutTable) / sizeof(int);*/

// new table
const int newMenuColLayoutTable[] = {
  // 1-column menu
  0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // 2-column menu
  0x00, 0x0E, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
  // 3-column menu
  // 0x40->0x48 (original text target: (14 * 5)  = 0x46
  // 0x88->0x90 (original text target: (14 * 10) = 0x8C)
  0x00, 0x08+0x1, 0x11+0x1, 0x1C, 0x00, 0x00, 0x00, 0x00,
  // 4-column menu (is this actually used? maybe for the settings menu?)
  0x00, 0x07, 0x0E, 0x15, 0x1C, 0x00, 0x00, 0x00,
  // 5-column menu (as above, is this actually used?)
  0x00, 0x05, 0x0B, 0x10, 0x17, 0x1C, 0x00, 0x00,
};
const int numNewMenuColLayoutTableEntries
  = sizeof(newMenuColLayoutTable) / sizeof(int);

void patchStdBlock(TStream& ofs, TenmaScriptReader::ResultRegion& region,
                   std::string name = "") {
  for (auto entry: region.strings) {
    // claim space for new entry, failing if not enough available
    int newPos = region.freeSpace.claim(entry.str.size());
    if (newPos < 0) {
      throw TGenericException(T_SRCANDLINE,
                              "patchStdBlock()",
                              std::string("Region '")
                              + name
                              + "': no space for string '"
                              + entry.id
                              + "'");
    }
    
    // write new data
    ofs.seek(newPos);
    ofs.writeString(entry.str);
    
    // update references
    for (auto pointerRef: entry.pointerRefs) {
      ofs.seek(pointerRef);
      ofs.writeu16le(newPos);
      
      // update to use new opcode
      // (we have to distinguish modified pointers from original ones
      // so the script base bank can be set to a lower area for the hack)
      ofs.seek(pointerRef - 1);
      ofs.writeu8(0xFE);
    }
  }
  
  // update menu column layout if applicable
  if (region.hasProperty("menuColLayoutTable_raw")) {
    int menuColLayoutTable_raw
      = TStringConversion::stringToInt(
          region.properties.at("menuColLayoutTable_raw"));
    ofs.seek(menuColLayoutTable_raw);
    for (int i = 0; i < numNewMenuColLayoutTableEntries; i++) {
      ofs.writeu8(newMenuColLayoutTable[i]);
    }
  }
}



void readScript(std::string filename, TenmaScriptReader::NameToRegionMap& dst,
                BlackT::TThingyTable& table) {
  TBufStream ifs;
  ifs.open(filename.c_str());
  TenmaScriptReader(ifs, dst, table)();
}



void freeRegionStringSpaces(TenmaScriptReader::ResultRegion& src,
                            TFreeSpace& dst) {
  // free space from all strings, minus the three-byte redirect sequence
  // that will be placed at the start of each one
  for (const auto& str: src.strings) {
    if (str.originalOffset < 0) continue;
    if (str.originalSize <= 3) continue;
    
    int freeStart = str.originalOffset + 3;
    int freeSize = str.originalSize - 3;
    dst.free(freeStart, freeSize);
  }
}

void freeRegionStringSpacesFully(TenmaScriptReader::ResultRegion& src,
                            TFreeSpace& dst) {
  // free space from all valid strings
  for (const auto& str: src.strings) {
    if (str.originalOffset < 0) continue;
    
    int freeStart = str.originalOffset;
    int freeSize = str.originalSize;
    dst.free(freeStart, freeSize);
  }
}

void freeRegionSet(TenmaScriptReader::NameToRegionMap& src,
                   TFreeSpace& dst) {
  for (auto& item: src) {
    freeRegionStringSpaces(item.second, dst);
  }
}

void redirectScriptBlock(TenmaScriptReader::ResultRegion& src,
                         BlackT::TStream& dst,
                         BlackT::TStream* globalDst,
                         int baseAddr,
                         TFreeSpace* localFreeSpace,
                         TFreeSpace* globalFreeSpace
                         ) {
  for (auto& str: src.strings) {
/*    if (str.originalOffset < 0) continue;
    if (str.originalSize < 3) {
      // TODO: warn?
      continue;
    }*/
    
    int neededSize = str.str.size() + 1;
    bool needsTerminator = true;
    if (str.propertyIsTrue("terminatedByPrompt")
        || str.propertyIsTrue("terminatedByF9")) {
      needsTerminator = false;
      neededSize = str.str.size();
    }
    bool isLocal = false;
    int dstAddr = -1;
    
    bool isDirectRedirect = false;
    int directRedirectPtrBase = -1;
    int directRedirectPtrOffset = -1;
    int directRedirectMsgId = -1;
    if (str.propertyIsTrue("isDirectRedirect")) {
      isDirectRedirect = true;
      
      directRedirectPtrBase = TStringConversion::stringToInt(
        str.properties.at("directRedirect_ptrBase"));
      directRedirectPtrOffset = TStringConversion::stringToInt(
        str.properties.at("directRedirect_ptrOffset"));
      
      directRedirectMsgId = TStringConversion::stringToInt(
        str.properties.at("directRedirect_msgId"));
      // need 1 extra byte for message id
      // (technically might need 3 depending on type but this is only
      // going to be applied to a limited subset of messages)
      ++neededSize;
    }
    
    if (!isDirectRedirect) {
      if (str.originalOffset < 0) continue;
      if (str.originalSize < 3) {
        // TODO: warn?
        continue;
      }
    }
    
    if ((dstAddr < 0) && (localFreeSpace != NULL)) {
      dstAddr = localFreeSpace->claim(neededSize);
      if (dstAddr >= 0) {
        isLocal = true;
      }
    }
    
    if ((dstAddr < 0) && (globalFreeSpace != NULL)) {
      dstAddr = globalFreeSpace->claim(neededSize);
      if (dstAddr >= 0) {
        isLocal = false;
      }
    }
    
    if (dstAddr < 0) {
/*      if ((localFreeSpace != NULL)) {
        std::cerr << std::hex << neededSize << std::endl;
        for (auto& it: localFreeSpace->freeSpace_) {
          std::cerr << std::hex << it.first << " " << std::hex << it.second << std::endl;
        }
      }*/
      
      std::string remainingSpaceStr;
      for (auto item: localFreeSpace->freeSpace_) {
        remainingSpaceStr += TStringConversion::intToString(
          item.first, TStringConversion::baseHex);
        remainingSpaceStr += ": ";
        remainingSpaceStr += TStringConversion::intToString(
          item.second, TStringConversion::baseHex);
        remainingSpaceStr += " bytes";
        remainingSpaceStr += "\n";
      }
      
      throw TGenericException(T_SRCANDLINE,
                              "redirectScriptBlock()",
                              std::string("No space for string '")
                              + str.id
                              + "'"
                              + "\n"
                              + "Needed space: "
                              + TStringConversion::intToString(
                                neededSize, TStringConversion::baseHex)
                              + "\n"
                              + "Free space map:\n"
                              + remainingSpaceStr);
    }
    
    TStream* dstContentTarget = NULL;
    if (isLocal) dstContentTarget = &dst;
    else dstContentTarget = globalDst;
    
    // write new content
    dstContentTarget->seek(dstAddr);
    if (isDirectRedirect) {
      dstContentTarget->writeu8(directRedirectMsgId);
    }
    dstContentTarget->writeString(str.str);
    if (needsTerminator)
      dstContentTarget->put(TenmaMsgConsts::opcode_end);
    
    if (isDirectRedirect) {
      // update pointer reference
//      std::cerr << str.id << ": "
//         << TStringConversion::intToString(
//                                dstAddr, TStringConversion::baseHex)
//                                << std::endl;
      dst.seek(directRedirectPtrOffset);
      dst.writeu16le(directRedirectPtrBase + dstAddr);
    }
    else {
      // write redirect sequence
      dst.seek(str.originalOffset);
      if (isLocal) {
        dst.writeu8(code_localStringRedirect);
        dst.writeu16le(baseAddr + dstAddr);
      }
      else {
        dst.writeu8(code_globalStringRedirect);
        dst.writeu16le(dstAddr);
      }
    }
  }
}
    
void redirectTabledScriptBlock(TenmaScriptReader::ResultRegion& src,
                         BlackT::TStream& dst,
                         BlackT::TStream* globalDst,
                         int tableStartOffset,
                         int tableSize,
                         int baseAddr,
                         TFreeSpace* localFreeSpace,
                         TFreeSpace* globalFreeSpace) {
//  int tableIndex = -1;
  
  std::map<int, int> ptrMap;
  for (auto& str: src.strings) {
//    ++tableIndex;
    if (str.originalOffset < 0) continue;
    
    bool needsTerminator = true;
    int neededSize = str.str.size() + 1;
    if (str.propertyIsTrue("noTerminator")) {
      needsTerminator = false;
      --neededSize;
    }
    
    int dstAddr = -1;
    if (localFreeSpace != NULL) {
      dstAddr = localFreeSpace->claim(neededSize);
    }
    
    if (dstAddr < 0) {
      throw TGenericException(T_SRCANDLINE,
                              "redirectTabledScriptBlock()",
                              std::string("No space for string '")
                              + str.id
                              + "'");
    }
    
    // write string
    dst.seek(dstAddr);
    dst.writeString(str.str);
    if (needsTerminator) {
      dst.writeu8(TenmaMsgConsts::opcode_end);
    }
    
    // write index table entry
//    dst.seek(tableStartOffset + (tableIndex * 2));
//    dst.writeu16le(dstAddr + baseAddr);
    
    ptrMap[str.originalOffset] = dstAddr;
  }
  
  // update index
  for (int i = 0; i < tableSize; i++) {
    int entryOffset = tableStartOffset + (i * 2);
    dst.seek(entryOffset);
    int oldPtr = dst.readu16le();
    
    int newPtr = ptrMap.at(oldPtr - baseAddr) + baseAddr;
    dst.seek(entryOffset);
    dst.writeu16le(newPtr);
  }
}

void redirectOneShotScriptBlock(TenmaScriptReader::ResultRegion& src,
                         BlackT::TStream& dst,
                         int baseAddr,
                         TFreeSpace* globalFreeSpace = NULL
                         ) {
  TFreeSpace localFreeSpace = src.freeSpace;
  freeRegionStringSpaces(src, localFreeSpace);
  redirectScriptBlock(src, dst, NULL, baseAddr,
                      &localFreeSpace, globalFreeSpace);
}

void redirectSharedScriptBlock(TenmaScriptReader::NameToRegionMap& src,
                         BlackT::TStream& dst,
                         int baseAddr,
                         TFreeSpace* globalFreeSpace = NULL
                         ) {
  TFreeSpace localFreeSpace;
  // use first region's free space if it exists
  if (src.size() > 0) localFreeSpace = src.begin()->second.freeSpace;
  
  for (auto& regionPair: src) {
    freeRegionStringSpaces(regionPair.second, localFreeSpace);
  }
  
  for (auto& regionPair: src) {
    redirectScriptBlock(regionPair.second, dst, NULL, baseAddr,
                        &localFreeSpace, globalFreeSpace);
  }
}

void redirectBossMessages(TenmaScriptReader::ResultRegion& src,
                 BlackT::TStream& isoIfs) {
  int originSector = TStringConversion::stringToInt(
    src.properties.at("originSector"));
//  std::cerr << std::hex << originSector << std::endl;
  
  int baseAddr = originSector * TenmaMsgConsts::sectorSize;
  TBufStream ifs;
  isoIfs.seek(baseAddr);
  // WARNING: this overreads for some altboss sectors.
  // probably won't cause any issues, though.
  // the proper size for the regular boss modules is 0x6800 bytes?
  // we only need the first part, though.
  ifs.writeFrom(isoIfs, 0x1000);
  
  redirectOneShotScriptBlock(src, ifs, 0xC000);
  
  isoIfs.seek(baseAddr);
//  ifs.seek(0);
  isoIfs.write(ifs.data().data(), ifs.size());
}

void redirectBossMessageSet(TenmaScriptReader::NameToRegionMap& src,
                 BlackT::TStream& isoIfs) {
  for (auto& regionPair: src) {
    redirectBossMessages(regionPair.second, isoIfs);
  }
}

void redirectMap(TenmaScriptReader::ResultRegion& src,
                 BlackT::TStream& isoIfs) {
  int mapBaseSector = TStringConversion::stringToInt(
    src.properties.at("mapStartSector"));
  int mapBaseOffset = mapBaseSector * TenmaMsgConsts::sectorSize;
  
  TFreeSpace freeSpace = src.freeSpace;
  
  // free old strings
  for (auto& str: src.strings) {
    // reserve the first 3 bytes for the redirect op
    int freeStart = str.originalOffset + 3;
    // reserve the final byte for a jump back to the old terminator
    // unless sequence is terminated with a prompt (which we do not
    // jump back to)
    int freeEnd = str.originalOffset + str.originalSize - 1;
    if (str.propertyIsTrue("terminatedByPrompt")
        || str.propertyIsTrue("terminatedByF9")) {
//      freeEnd = str.originalOffset + str.originalSize - 5;
      freeEnd = str.originalOffset + str.originalSize;
    }
    
    if (freeEnd > freeStart)
      freeSpace.free(freeStart, freeEnd - freeStart);
  }
  
  // redirect strings
  for (auto& str: src.strings) {
    if (str.originalOffset < 0) continue;
    if (str.originalSize < 3) {
      // TODO: warn?
      continue;
    }
    
    int neededSize = str.str.size() + 3;
    bool needsTerminator = false;
    bool jumpBackToTerminator = true;
    if (str.propertyIsTrue("terminatedByPrompt")
        || str.propertyIsTrue("terminatedByF9")) {
      needsTerminator = false;
      jumpBackToTerminator = false;
      neededSize = str.str.size();
    }
    else if (str.originalSize < 4) {
      // TODO: warn for case where originalSize < 4?
      // (cannot jump back to terminator in that case because it
      // will be overwritten by the redirect sequence)
      needsTerminator = true;
      jumpBackToTerminator = false;
      neededSize = str.str.size() + 1;
    }
    
    int dstAddr = freeSpace.claim(neededSize);
    
    if (dstAddr < 0) {
      throw TGenericException(T_SRCANDLINE,
                              "redirectMap()",
                              std::string("No space for string '")
                              + str.id
                              + "'");
    }
    
    // write new content
    isoIfs.seek(mapBaseOffset + dstAddr);
    isoIfs.writeString(str.str);
    
    // add termination sequence if applicable
    if (needsTerminator) {
      isoIfs.writeu8(TenmaMsgConsts::opcode_end);
    }
    else if (jumpBackToTerminator) {
      isoIfs.writeu8(code_localStringRedirect);
      isoIfs.writeu16le(0xC000 + str.originalOffset + str.originalSize - 1);
    }
    
    // write redirect sequence
    isoIfs.seek(mapBaseOffset + str.originalOffset);
    isoIfs.writeu8(code_localStringRedirect);
    isoIfs.writeu16le(0xC000 + dstAddr);
  }
}

void exportSeqRegion(TenmaScriptReader::ResultCollection& results,
                     TBufStream& dst) {
  for (TenmaScriptReader::ResultCollection::iterator it = results.begin();
       it != results.end();
       ++it) {
//    if (it->str.size() <= 0) continue;
    
    TenmaScriptReader::ResultString& str = *it;
    dst.writeString(str.str);
    
    if (str.propertyIsTrue("terminatedByPrompt")
        || str.propertyIsTrue("terminatedByF9")) {
    
    }
    else if (!str.propertyIsTrue("noTerminator")) {
      dst.writeu8(TenmaMsgConsts::opcode_end);
    }
  }
}

void exportSeqRegionMap(TenmaScriptReader::NameToRegionMap& results,
                        TBufStream& dst) {
  for (auto it: results) {
    exportSeqRegion(it.second.strings, dst);
  }
}

void exportItemTableInc(TenmaScriptReader::ResultRegion& region,
                        std::string filename) {
  TFileManip::createDirectoryForFile(filename);
  std::ofstream ofs(filename.c_str());
  
  ofs << ".bank metabank_kernel slot std_slot" << std::endl;
  ofs << ".section \"item table index\" free" << std::endl;
  ofs << "  itemTableIndex:" << std::endl;
  {
    int itemNum = 0;
    for (auto& item: region.strings) {
      ofs << "  .db opcode_globalRedirect" << std::endl;
      ofs << "  .dw itemTableEntry_" << itemNum << std::endl;
      ++itemNum;
    }
  }
  ofs << ".ends" << std::endl << std::endl;
  
  ofs << ".bank metabank_new2 slot std_slot" << std::endl;
  ofs << ".section \"item table data\" free" << std::endl;
  {
    int itemNum = 0;
    for (auto& item: region.strings) {
      ofs << "  itemTableEntry_" << itemNum << ":" << std::endl;
      TBufStream temp;
      temp.writeString(item.str);
      temp.writeu8(TenmaMsgConsts::opcode_end);
      temp.seek(0);
      binToDcb(temp, ofs);
      ++itemNum;
    }
  }
  ofs << ".ends" << std::endl << std::endl;
}

const static int numEnemyEntries = 193;
const static int enemyEntrySize = 0x30;
const static int battleExtraAdpcmLoadAddr = 0x5000;
const static int battleExtraNameBlockStart = numEnemyEntries * enemyEntrySize;

struct EnemyNameData {
  int newOffset;
  int newSize;
};

void regenerateEnemyNameData(TenmaScriptReader::ResultRegion& src,
                         BlackT::TStream& dst) {
  std::map<int, EnemyNameData> nameMap;
  dst.seek(battleExtraNameBlockStart);
  for (auto& str: src.strings) {
    EnemyNameData data;
    data.newOffset = dst.tell();
    data.newSize = str.str.size() + 1;
    nameMap[str.originalOffset] = data;
    
    dst.writeString(str.str);
    dst.writeu8(TenmaMsgConsts::opcode_end);
  }
  
  for (int i = 0; i < numEnemyEntries; i++) {
    int basePos = (enemyEntrySize * i);
    
    dst.seek(basePos + 0);
    int oldNameOffset = dst.readu16le() - battleExtraAdpcmLoadAddr;
    
    std::map<int, EnemyNameData>::iterator findIt
      = nameMap.find(oldNameOffset);
    if (findIt == nameMap.end()) {
      // this is possible; four unused entries have invalid "(null)" strings
      // which were omitted from the dumped script.
      // we just ignore this case.
      continue;
    }
    
    dst.seek(basePos + 0);
    dst.writeu16le(findIt->second.newOffset + battleExtraAdpcmLoadAddr);
    dst.seek(basePos + 0x29);
    dst.writeu8(findIt->second.newSize);
    
    if (findIt->second.newSize > 16) {
      // TODO: warn?
    }
  }
  
  if (dst.size() > 0x3000) {
    throw TGenericException(T_SRCANDLINE,
                            "regenerateEnemyNameData()",
                            std::string("Overran enemy name block"));
  }
}



int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Tengai Makyou Ziria script builder" << endl;
    cout << "Usage: " << argv[0] << " [inprefix] [outprefix]"
      << endl;
    return 0;
  }
  
//  string infile(argv[1]);
  string inPrefix(argv[1]);
  string outPrefix(argv[2]);

  tableStd.readUtf8("table/tenma_std_en.tbl");
  tableMenu.readUtf8("table/tenma_menu_en.tbl");
  tableScene.readUtf8("table/tenma_scenes_en.tbl");
  
/*  tableScene.readSjis("table/tenma_scenes_en.tbl");
  table8x8.readSjis("table/ascii.tbl");
  tableCredits.readSjis("table/tenma_credits_en.tbl");
  tableBackUtil.readSjis("table/tenma_backutil_en.tbl");*/
  
  //=====
  // read script
  //=====
  
  TenmaScriptReader::NameToRegionMap battleResults;
  TenmaScriptReader::NameToRegionMap bossResults;
  TenmaScriptReader::NameToRegionMap altBossResults;
  TenmaScriptReader::NameToRegionMap creditsResults;
  TenmaScriptReader::NameToRegionMap dialogueResults;
  TenmaScriptReader::NameToRegionMap enemyResults;
  TenmaScriptReader::NameToRegionMap genericTextResults;
  TenmaScriptReader::NameToRegionMap infoResults;
  TenmaScriptReader::NameToRegionMap introEndingResults;
  TenmaScriptReader::NameToRegionMap itemsResults;
  TenmaScriptReader::NameToRegionMap menuResults;
  TenmaScriptReader::NameToRegionMap miscResults;
  TenmaScriptReader::NameToRegionMap saveLoadResults;
  TenmaScriptReader::NameToRegionMap shopResults;
  TenmaScriptReader::NameToRegionMap subtitleResults;
  
  readScript((inPrefix + "spec_battle.txt"), battleResults, tableStd);
  readScript((inPrefix + "spec_boss.txt"), bossResults, tableStd);
  readScript((inPrefix + "spec_bossalt.txt"), altBossResults, tableStd);
  readScript((inPrefix + "spec_credits.txt"), creditsResults, tableStd);
  readScript((inPrefix + "spec_dialogue.txt"), dialogueResults, tableStd);
  readScript((inPrefix + "spec_enemy.txt"), enemyResults, tableStd);
  readScript((inPrefix + "spec_generic_text.txt"), genericTextResults, tableStd);
  readScript((inPrefix + "spec_info.txt"), infoResults, tableStd);
  readScript((inPrefix + "spec_intro_ending.txt"), introEndingResults, tableStd);
  readScript((inPrefix + "spec_items.txt"), itemsResults, tableStd);
  readScript((inPrefix + "spec_menu.txt"), menuResults, tableMenu);
  readScript((inPrefix + "spec_misc.txt"), miscResults, tableStd);
  readScript((inPrefix + "spec_saveload.txt"), saveLoadResults, tableStd);
  readScript((inPrefix + "spec_shop.txt"), shopResults, tableStd);
  readScript((inPrefix + "spec_subtitle.txt"), subtitleResults, tableScene);
  
  //=====
  // read files to be updated
  //=====
  
  TBufStream isoIfs;
  isoIfs.open("tenma_02_build.iso");
  
  TBufStream battleIfs;
  battleIfs.open("out/base/battle_1D.bin");
  TBufStream battleExtraIfs;
  battleExtraIfs.open("out/base/battle_extra_27.bin");
  TBufStream battleTextIfs;
  battleTextIfs.open("out/base/battle_text_2D.bin");
  TBufStream battleText2Ifs;
  battleText2Ifs.open("out/base/battle_text2_31.bin");
  TBufStream creditsIfs;
  creditsIfs.open("out/base/credits_4F.bin");
//  TBufStream emptyIfs;
//  emptyIfs.open("out/base/empty_2.bin");
  TBufStream genericTextIfs;
  genericTextIfs.open("out/base/generic_text_4B.bin");
  TBufStream infoIfs;
  infoIfs.open("out/base/info_5B.bin");
  TBufStream introIfs;
  introIfs.open("out/base/intro_3B.bin");
  TBufStream kernelIfs;
  kernelIfs.open("out/base/kernel_9.bin");
  TBufStream overwIfs;
  overwIfs.open("out/base/overw_13.bin");
  TBufStream saveLoadIfs;
  saveLoadIfs.open("out/base/saveload_45.bin");
  TBufStream shopIfs;
  shopIfs.open("out/base/shop_19.bin");
  TBufStream specialIfs;
  specialIfs.open("out/base/special_51.bin");
  TBufStream unknown1Ifs;
  unknown1Ifs.open("out/base/unknown1_23.bin");
  TBufStream unknown2Ifs;
  unknown2Ifs.open("out/base/unknown2_35.bin");
  TBufStream unknown3Ifs;
  unknown3Ifs.open("out/base/unknown3_57.bin");
  
  //=====
  // compress
  //=====
  
    {
      TenmaScriptReader::NameToRegionMap allStrings;
      
      std::vector<TenmaScriptReader::NameToRegionMap*> allSrcPtrs;
      allSrcPtrs.push_back(&battleResults);
//      allSrcPtrs.push_back(&creditsResults);
      allSrcPtrs.push_back(&dialogueResults);
      allSrcPtrs.push_back(&enemyResults);
      allSrcPtrs.push_back(&genericTextResults);
      allSrcPtrs.push_back(&infoResults);
      allSrcPtrs.push_back(&introEndingResults);
      allSrcPtrs.push_back(&itemsResults);
      allSrcPtrs.push_back(&saveLoadResults);
      allSrcPtrs.push_back(&shopResults);
      
      // merge everything into one giant map for compression
      mergeResultMaps(allSrcPtrs, allStrings);
      
      #if BUILD_COMPRESSION_DICTIONARY
      {
        // compress
        generateCompressionDictionary(
          allStrings, outPrefix + "script_dictionary.bin");
      }
      #else
      {
        TBufStream dictIfs;
        dictIfs.open((outPrefix + "script_dictionary.bin").c_str());
        
        int index = 0;
        while (!dictIfs.eof()) {
          int symbol = index + textCharsEnd;
          std::string srcStr;
          srcStr += dictIfs.get();
          srcStr += dictIfs.get();
          applyDictionaryEntry(srcStr,
                               allStrings,
                               std::string() + (char)symbol);
          ++index;
        }
      }
      #endif
      
      // restore results from merge back to individual containers
      unmergeResultMaps(allStrings, allSrcPtrs);
    }
  
  //=====
  // update blocks
  //=====
  
//  redirectOneShotScriptBlock(saveLoadResults.begin()->second,
//                             saveLoadIfs,
//                             0x9000);
  redirectSharedScriptBlock(saveLoadResults,
                            saveLoadIfs,
                            0x9000);
  
//  redirectOneShotScriptBlock(introEndingResults.begin()->second,
//                             introIfs,
//                             0x9000);
  redirectSharedScriptBlock(introEndingResults,
                            introIfs,
                            0x9000);
  
//  redirectSharedScriptBlock(genericTextResults,
//                            genericTextIfs,
//                            0xC000);
  {
    TFreeSpace freeSpace = genericTextResults.begin()->second.freeSpace;
//    freeRegionSet(genericTextResults, freeSpace);
    freeRegionStringSpaces(genericTextResults.at("generic_msg"),
                           freeSpace);
    freeRegionStringSpacesFully(genericTextResults.at("generic_techname"),
                           freeSpace);
    
//    for (auto it: freeSpace.freeSpace_) {
//      std::cerr << std::hex << it.first << " " << it.second << std::endl;
//    }
    
    redirectScriptBlock(genericTextResults.at("generic_msg"),
                        genericTextIfs, NULL, 0xC000,
                        &freeSpace, NULL);
    
    redirectTabledScriptBlock(genericTextResults.at("generic_techname"),
                        genericTextIfs, NULL,
                        0xF17, 0x2A, 0xC000,
                        &freeSpace, NULL);
  }
  
  redirectOneShotScriptBlock(battleResults["battle_std"],
                             battleTextIfs,
                             0xC000);
  redirectOneShotScriptBlock(battleResults["battle_boss"],
                             battleText2Ifs,
                             0xC000);
  
  redirectOneShotScriptBlock(miscResults.at("misc_gokumon"),
                             specialIfs,
                             0x9000);
  
  redirectSharedScriptBlock(shopResults,
                            shopIfs,
                            0xC000);
  
  redirectSharedScriptBlock(infoResults,
                            infoIfs,
                            0xC000);
  
  redirectBossMessageSet(bossResults, isoIfs);
  redirectBossMessageSet(altBossResults, isoIfs);
  
  //=====
  // export generic/hardcoded strings
  //=====
  
/*  {
    TBufStream itemTable;
    exportSeqRegionMap(itemsResults, itemTable);
    TFileManip::createDirectory("out/script/items");
    itemTable.save("out/script/items/itemtable.bin");
  }*/
  
  exportItemTableInc(itemsResults.begin()->second,
                     "out/script/items/itemlist.inc");
  
  exportGenericRegion(miscResults["misc_yesno_prompt"].strings,
                      "out/script/misc/");
  
  exportGenericRegion(menuResults["menu_kernel"].strings,
                      "out/script/menu/kernel/");
  
  exportGenericRegion(menuResults["menu_overw"].strings,
                      "out/script/menu/overw/");
  
  exportGenericRegion(menuResults["menu_shop"].strings,
                      "out/script/menu/shop/");
  
  exportGenericRegion(menuResults["menu_battle"].strings,
                      "out/script/menu/battle/");
  
  exportGenericRegion(menuResults["menu_saveload"].strings,
                      "out/script/menu/saveload/");
  
  exportGenericRegion(menuResults["menu_info"].strings,
                      "out/script/menu/info/");
  
  exportGenericRegion(menuResults["orochimaru_name"].strings,
                      "out/script/menu/orochimaru_patch/");
  
//  exportGenericString(menuResults["orochimaru_name"].strings["str-0x5B7"],
//                      "out/script/orochimaru_patch/");
//  exportGenericString(menuResults["orochimaru_name"].strings["str-0x5C7"],
//                      "out/script/orochimaru_patch/");
  
  //=====
  // regenerate enemy name data
  //=====
  
  regenerateEnemyNameData(enemyResults.begin()->second,
                          battleExtraIfs);
  
  //=====
  // update maps
  //=====
  
  for (auto& nameRegionPair: dialogueResults) {
    TenmaScriptReader::ResultRegion& region
      = nameRegionPair.second;
    std::cerr << "redirecting map: " << nameRegionPair.first << std::endl;
    redirectMap(region, isoIfs);
  }
  
/*  for (auto entry: scriptResults) {
    std::string name = entry.first;
    TenmaScriptReader::ResultRegion& region = entry.second;
    
    std::string filename;
    if (region.regionType == TenmaScriptReader::regionType_area) {
      filename = std::string("out/base/area/") + name + ".bin";
    }
    else if (region.regionType == TenmaScriptReader::regionType_adv) {
      filename = std::string("out/base/adv/") + name + ".bin";
    }
    
    if (!filename.empty()) {
      TBufStream ifs;
      ifs.open(filename.c_str());
      patchStdBlock(ifs, region, name);
      ifs.save(filename.c_str());
    }
  }
  
  //=====
  // export generic/hardcoded strings
  //=====
  
  exportGenericRegionMap(advSceneResults, "out/script/strings/");
  exportGenericRegionMap(visualResults, "out/script/strings/");
  exportGenericRegionMap(miscResults, "out/script/strings/");
  exportGenericRegionMap(t8x8Results, "out/script/strings/");
  exportGenericRegionMap(creditsResults, "out/script/strings/");
//  exportGenericRegionMap(creditsTextResults, "out/script/strings/");
  
  //=====
  // output auto-generated ASM includes for scenes
  //=====
  
  exportAdvSceneIncludes(advSceneResults, "asm/gen/");
  exportVisualIncludes(visualResults, "asm/gen/");
  
  //=====
  // output auto-generated ASM includes for credits
  //=====
  
  exportCreditsIncludes(creditsTextResults, "asm/gen/");
  
  //=====
  // output auto-generated ASM includes for backup utility
  //=====
  
  exportBackUtilIncludes(backUtilResults, "asm/gen/");*/
  
  //=====
  // export subtitles
  //=====
  
  exportGenericRegion(subtitleResults.begin()->second.strings,
                      "out/script/subtitle/");
  
  //=====
  // export credits
  //=====
  
  exportGenericRegion(creditsResults.begin()->second.strings,
                      "out/script/credits/");
  
  // compute lengths of credits strings and output an asm include
  // indicating the x-position needed to center them, purely for convenience
  {
    TBitmapFont font;
    font.load("font/tall/");
    
    std::ofstream ofs("out/script/credits/centerwidths.inc");
    
    for (auto& item: creditsResults.begin()->second.strings) {
      TenmaScriptReader::ResultString& str = item;
      TBufStream ifs;
      ifs.open((std::string("out/script/credits/") + str.id + ".bin").c_str());
      
      int length = 0;
      while (!ifs.eof()) {
        int next = ifs.readu8();
        if (next == 0xFF) break;
        
        int charOffset = next - textCharsStart;
        length += font.fontChar(charOffset).advanceWidth;
      }
      
      std::string idStr = str.id;
      for (int i = 0; i < idStr.size(); i++) {
        if (idStr[i] == '-') idStr[i] = '_';
      }
      
      ofs << ".define credCenterX_" << idStr << " "
        << ((256 - length) / 2) << std::endl;
    }
  }
  
  //=====
  // save modified files
  //=====
  
  battleIfs.save("out/base/battle_1D.bin");
  battleExtraIfs.save("out/base/battle_extra_27.bin");
  battleTextIfs.save("out/base/battle_text_2D.bin");
  battleText2Ifs.save("out/base/battle_text2_31.bin");
  creditsIfs.save("out/base/credits_4F.bin");
//  emptyIfs.save("out/base/empty_2.bin");
  genericTextIfs.save("out/base/generic_text_4B.bin");
  infoIfs.save("out/base/info_5B.bin");
  introIfs.save("out/base/intro_3B.bin");
  kernelIfs.save("out/base/kernel_9.bin");
  overwIfs.save("out/base/overw_13.bin");
  saveLoadIfs.save("out/base/saveload_45.bin");
  shopIfs.save("out/base/shop_19.bin");
  specialIfs.save("out/base/special_51.bin");
  unknown1Ifs.save("out/base/unknown1_23.bin");
  unknown2Ifs.save("out/base/unknown2_35.bin");
  unknown3Ifs.save("out/base/unknown3_57.bin");
  
  //=====
  // save modified iso
  //=====
  
  isoIfs.save("tenma_02_build.iso");
  
  return 0;
}
