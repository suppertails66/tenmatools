//=====================================================
// Tool to do initial setup for the project from the
// base ISO (expected to be named tenma_02.iso
// and placed in the same directory as this program)
//=====================================================

#include "pce/okiadpcm.h"
#include "pce/PcePalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TFileManip.h"
#include "util/TStringSearch.h"
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;
using namespace BlackT;
using namespace Pce;

#define VERIFY_MAP_HEADERS 1

const static int sectorSize = 0x800;

//std::ostream& outOfs = std::cout;
std::ofstream outOfs;

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}

std::string getHexNumStrNoPrefix(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  return str;
}

//const static int numMaps = 92;
const static int numMaps = 128;
const static int mapBaseSector = 0x1137;
// the game loads 0x1A sectors to the adpcm buffer for processing...
// what are these 2 extra sectors?
//const static int mapSectorLoadSize = 0x1A;
const static int mapSectorStdLoadSize = 0x1C;
const static int mapSectorStdSpacing = 0x1C;
//const static int numMapCheckWords = 14;
const static int numMapCheckWords = 14;
// used by some maps later in the rom
const static int mapSectorShortSpacing = 0x1A;
const static int mapSectorShortLoadSize = 0x1A;

void dumpSectorData(TStream& ifs, int sectorNum, int sectorCount,
                    std::string outFileBase) {
  std::string outName = std::string("base/")
    + outFileBase
    + "_"
    + TStringConversion::intToString(sectorNum,
        TStringConversion::baseHex).substr(2, std::string::npos)
    + ".bin";
  TFileManip::createDirectoryForFile(outName);
  
  TBufStream ofs;
  ifs.seek(sectorNum * sectorSize);
  ofs.writeFrom(ifs, sectorCount * sectorSize);
  ofs.save(outName.c_str());
}

int currentMapNum = 0;
void dumpMapSectorData(TStream& ifs, int sectorNum, int sectorCount,
                    std::string outFileBase) {
  std::string outName = std::string("base/")
    + outFileBase
    + "_"
    + TStringConversion::intToString(sectorNum,
        TStringConversion::baseHex).substr(2, std::string::npos)
    + ".bin";
  TFileManip::createDirectoryForFile(outName);
  
  TBufStream ofs;
  ifs.seek(sectorNum * sectorSize);
  ofs.writeFrom(ifs, sectorCount * sectorSize);
  
  #if VERIFY_MAP_HEADERS != 0
  // verify header pointers
    ofs.seek(0);
    bool nonzeroPtrExisted = false;
    for (int i = 0; i < numMapCheckWords; i++) {
      int ptr = ofs.readu16le();
      if (ptr == 0) continue;
  //    if (ptr == 0xFFFF) continue;
      
      if ((ptr < 0xC000) || (ptr >= 0xE000)) {
        ofs.save(outName.c_str());
        throw TGenericException(T_SRCANDLINE,
                                "dumpSectorData()",
                                outName
                                + ": bad map header pointer");
      }
      nonzeroPtrExisted = true;
    }
    
    if (!nonzeroPtrExisted) {
      ofs.save(outName.c_str());
      throw TGenericException(T_SRCANDLINE,
                              "dumpSectorData()",
                              outName
                              + ": null map header");
    }
  #endif
  
/*  // TEST: print map name
  ofs.seek(0x16);
  int namePtr = ofs.readu16le();
  int nameOffset = namePtr - 0xC000;
  ofs.seek(nameOffset + 1);
  std::cout << "map " << std::dec << currentMapNum
//    << " (" << nameOffset << ")"
    << " (sector 0x" << std::hex << sectorNum << ")"
    << ": ";
  while ((unsigned char)ofs.peek() != 0xFF) {
    std::cout << ofs.get();
    std::cout << ofs.get();
  }
  std::cout << std::endl; */
  
  ofs.save(outName.c_str());
}

void dumpMapData(TStream& ifs, int sectorNum, int sectorCount) {
  std::string outName = std::string("map")
    + TStringConversion::intToString(currentMapNum);
  dumpMapSectorData(ifs, sectorNum, sectorCount, outName);
  ++currentMapNum;
}

int main(int argc, char* argv[]) {
  TBufStream ifs;
  ifs.open("tenma_02.iso");
  
  dumpSectorData(ifs, 0x2, 0x7, "empty");
  dumpSectorData(ifs, 0x9, 0xA, "kernel");
  dumpSectorData(ifs, 0x13, 0x6, "overw");
  dumpSectorData(ifs, 0x19, 0x4, "shop");
  dumpSectorData(ifs, 0x1D, 0x6, "battle");
  // 0x23 -- 4 sectors of ???
  dumpSectorData(ifs, 0x23, 0x4, "unknown1");
  dumpSectorData(ifs, 0x27, 0x6, "battle_extra");
  // TODO: this may actually be two separate segments
//  dumpSectorData(ifs, 0x2D, 0x8, "battle_text");
  dumpSectorData(ifs, 0x2D, 0x4, "battle_text");
  dumpSectorData(ifs, 0x31, 0x4, "battle_text2");
  // 0x35 -- 6 sectors of ???
  dumpSectorData(ifs, 0x35, 0x6, "unknown2");
  dumpSectorData(ifs, 0x3B, 0xA, "intro");
  dumpSectorData(ifs, 0x45, 0x6, "saveload");
  dumpSectorData(ifs, 0x4B, 0x4, "generic_text");
  dumpSectorData(ifs, 0x4F, 0x2, "credits");
  dumpSectorData(ifs, 0x51, 0x6, "special");
  // 0x57 -- 4 sectors of ???
  dumpSectorData(ifs, 0x57, 0x4, "unknown3");
  dumpSectorData(ifs, 0x5B, 0x4, "info");
  
/*  for (int i = 0; i < 64; i++) {
//    dumpSectorData(ifs, mapBaseSector + (i * mapSectorSpacing),
//                   mapSectorLoadSize, "map");
    dumpMapData(ifs, 0x1137 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 69; i++) {
//  for (int i = 0; i < 128; i++) {
    dumpMapData(ifs, 0x19B7 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 10; i++) {
//  for (int i = 0; i < 32; i++) {
    dumpMapData(ifs, 0x2144 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 62; i++) {
//  for (int i = 0; i < 64; i++) {
    dumpMapData(ifs, 0x2434 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 23; i++) {
//  for (int i = 0; i < 64; i++) {
    dumpMapData(ifs, 0x2B17 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 73; i++) {
//  for (int i = 0; i < 96; i++) {
    dumpMapData(ifs, 0x2FA8 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 65; i++) {
//  for (int i = 0; i < 96; i++) {
    dumpMapData(ifs, 0x3915 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 92; i++) {
//  for (int i = 0; i < 160; i++) {
    dumpMapData(ifs, 0x42ED + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 56; i++) {
//  for (int i = 0; i < 96; i++) {
    dumpMapData(ifs, 0x526F + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 36; i++) {
//  for (int i = 0; i < 96; i++) {
    dumpMapData(ifs, 0x5A33 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 54; i++) {
//  for (int i = 0; i < 256; i++) {
    dumpMapData(ifs, 0x5F88 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  // end-block maps
  
  for (int i = 0; i < 1; i++) {
    dumpMapData(ifs, 0xDD9F + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 15; i++) {
    dumpMapData(ifs, 0xE103 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  for (int i = 0; i < 19; i++) {
    dumpMapData(ifs, 0xE4D3 + (i * mapSectorStdSpacing),
                   mapSectorStdLoadSize);
  }
  
  // ??? is this actually valid??
//  for (int i = 0; i < 1; i++) {
//    dumpMapData(ifs, 0xE697 + (i * mapSectorStdSpacing),
//                   mapSectorStdLoadSize);
//  }
  
  // 0xE839
//  for (int i = 0; i < 1; i++) {
//    dumpMapData(ifs, 0xE6B3 + (i * mapSectorShortSpacing),
//                   mapSectorShortLoadSize);
//  }
  
  // ??? what is this??
//  for (int i = 0; i < 1; i++) {
//    dumpMapData(ifs, 0xE6CF + (i * mapSectorStdSpacing),
//                   mapSectorStdLoadSize);
//  }
  
  for (int i = 0; i < 14; i++) {
    dumpMapData(ifs, 0xE6E7 + (i * mapSectorShortSpacing),
                   mapSectorShortLoadSize);
  }*/
  
  return 0;
}
