#include "tenma/TenmaMsgConsts.h"
#include "pce/PcePattern.h"
#include "pce/PcePalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TOpt.h"
#include "exception/TGenericException.h"
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;
using namespace BlackT;
using namespace Pce;

const static int sectorSize = 0x800;

// unit: metatiles
const static int metablockWInMetatiles = 16;
const static int metablockHInMetatiles = 16;
const static int metatileWInPixels = 16;
const static int metatileHInPixels = 16;
const static int metablockWInPixels = metablockWInMetatiles * metatileWInPixels;
const static int metablockHInPixels = metablockHInMetatiles * metatileHInPixels;
const static int mapWInMetablocks = 6;
const static int mapHInMetablocks = 6;
const static int mapWInMetatiles = mapWInMetablocks * metablockWInMetatiles;
const static int mapHInMetatiles = mapHInMetablocks * metablockHInMetatiles;
const static int mapWInPixels = mapWInMetatiles * metatileWInPixels;
const static int mapHInPixels = mapHInMetatiles * metatileHInPixels;
const static int metatileSizeInBytes = 6;

const static int numPatternsInGrpBlock = 0x200;

static bool oldMapMode = false;

int fetchNextMetablockBit(TStream& ifs, int& bitMask) {
  int result = 0;
  if (((unsigned char)ifs.peek() & bitMask) != 0) result = 1;
  
  if (bitMask == 0x01) {
    bitMask = 0x80;
    ifs.get();
  }
  else bitMask >>= 1;
  
  return result;
}

int fetchMetablockValue(TStream& ifs, int& bitMask, int numBits) {
  int result = 0;
  
  for (int i = 0; i < numBits; i++) {
    result <<= 1;
    result |= fetchNextMetablockBit(ifs, bitMask);
  }
  
  return result;
}

/*U; readNextMapStructCmd
; 
; read value info from map stream
; $BB-BC = src
; sets:
; $49 to metatile(?) value
; $53 to repeat count? (1 by default)

009229  A9 01                lda #$01
00922B  85 53                sta $0053
; fetch a 2-bit value
00922D  A2 02                ldx #$02
00922F  20 D9 B5             jsr fetchBitstreamValue [$B5D9]
009232  C9 02                cmp #$02
009234  90 17                bcc [$924D]
; if >= 2
  009236  D0 12                bne [$924A]
  ; if 2
    ; fetch a 5-bit value
    009238  A2 05                ldx #$05
    00923A  20 D9 B5             jsr fetchBitstreamValue [$B5D9]
    00923D  18                   clc 
    00923E  69 02                adc #$02
    ; repeat count = result + 2
    009240  85 53                sta $0053
    ; fetch a 7-bit value
    009242  A2 07                ldx #$07
    009244  20 D9 B5             jsr fetchBitstreamValue [$B5D9]
    ; metatile num = read value
    009247  85 49                sta $0049
    009249  60                   rts 
  ; else if 3
    ; new metatile num is old value + 1
    00924A  E6 49                inc $0049
    00924C  60                   rts 
; else if < 2
  ; rotate into top 2 bits
  00924D  18                   clc 
  00924E  6A                   ror 
  00924F  6A                   ror 
  009250  6A                   ror 
  009251  85 49                sta $0049
  ; add a 6-bit value to get result
  009253  A2 06                ldx #$06
  009255  20 D9 B5             jsr fetchBitstreamValue [$B5D9]
  009258  05 49                ora $0049
  00925A  85 49                sta $0049
  00925C  60                   rts */

void fetchNextMetablockCmd(TStream& ifs, int& bitMask,
    int& repeatCount, unsigned char& repeatValue) {
  // default count is 1
  repeatCount = 1;
  
  // fetch a 2-bit value
  int cmd = fetchMetablockValue(ifs, bitMask, 2);
  switch (cmd) {
  case 0:
  case 1:
    repeatValue = (cmd << 6) | fetchMetablockValue(ifs, bitMask, 6);
    break;
  case 2:
    repeatCount = fetchMetablockValue(ifs, bitMask, 5) + 2;
    repeatValue = fetchMetablockValue(ifs, bitMask, 7);
    break;
  case 3:
    // note that repeatValue is 8-bit and wraps around at 0xFF
    ++repeatValue;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "fetchNextMetablockCmd()",
                            "bad cmd");
    break;
  }
  
}

void decompressMetablockData(TStream& ifs, TStream& ofs) {
  int bitMask = 0x80;
  int repeatCount = 0;
  unsigned char repeatValue = 0;
  while (ofs.size() < (metablockWInMetatiles * metablockHInMetatiles)) {
    if (repeatCount <= 0) {
      fetchNextMetablockCmd(ifs, bitMask, repeatCount, repeatValue);
    }
    else {
      ofs.put(repeatValue);
      --repeatCount;
    }
  }
}
  
void renderMetatile(TStream& metatileIfs,
    const std::vector<PcePattern>& patterns, const PcePalette& palette,
    TGraphic& grp, int baseX, int baseY) {
//  bool test = false;
//  if (metatileIfs.tell() != 0x52) test = true;
  
  int ul = metatileIfs.readu8();
  int ur = metatileIfs.readu8();
  int ll = metatileIfs.readu8();
  int lr = metatileIfs.readu8();
  
  int pal_ul = ((unsigned char)metatileIfs.peek() & 0xF0) >> 4;
  int pal_ur = ((unsigned char)metatileIfs.get() & 0x0F);
  int pal_ll = ((unsigned char)metatileIfs.peek() & 0xF0) >> 4;
  int pal_lr = ((unsigned char)metatileIfs.get() & 0x0F);
  
  if ((pal_ul & 0x08) != 0) ul |= 0x100;
  if ((pal_ur & 0x08) != 0) ur |= 0x100;
  if ((pal_ll & 0x08) != 0) ll |= 0x100;
  if ((pal_lr & 0x08) != 0) lr |= 0x100;
  
  pal_ul &= 0x7;
  pal_ur &= 0x7;
  pal_ll &= 0x7;
  pal_lr &= 0x7;
  
  patterns.at(ul).toGraphic(grp, baseX + 0, baseY + 0,
    &palette.paletteLines[pal_ul]);
  patterns.at(ur).toGraphic(grp, baseX + 8, baseY + 0,
    &palette.paletteLines[pal_ur]);
  patterns.at(ll).toGraphic(grp, baseX + 0, baseY + 8,
    &palette.paletteLines[pal_ll]);
  patterns.at(lr).toGraphic(grp, baseX + 8, baseY + 8,
    &palette.paletteLines[pal_lr]);
}

void renderMap(TStream& mapIfs, TGraphic& grp) {
  grp.resize(mapWInPixels, mapHInPixels);
  
  // read tile graphics
  std::vector<PcePattern> tileGraphics;
  mapIfs.seek(0x8000);
  if (oldMapMode) mapIfs.seek(0x4000);
  for (int i = 0; i < numPatternsInGrpBlock; i++) {
    PcePattern pattern;
    pattern.read(mapIfs);
    tileGraphics.push_back(pattern);
  }
  
  // read struct block
  TBufStream structIfs;
  mapIfs.seek(0x2000);
  structIfs.writeFrom(mapIfs, 0x2000);
  
  // read palette from struct block
  PcePalette palette;
  structIfs.seek(0x8);
  int palOffset = structIfs.readu16le() - 0xC000;
  structIfs.seek(palOffset);
  for (int i = 0; i < 0x8; i++) {
    palette.paletteLines[i].read(structIfs);
  }
  
  structIfs.seek(0x2);
  int metatileBlockOffset = structIfs.readu16le() - 0xC000;
  
  structIfs.seek(0x0);
  int metablockStructPtrOffset = structIfs.readu16le() - 0xC000;
  for (int j = 0; j < mapHInMetablocks; j++) {
    for (int i = 0; i < mapWInMetablocks; i++) {
      int targetBlock = (j * mapHInMetablocks) + i;
      
      structIfs.seek(metablockStructPtrOffset + (targetBlock * 2));
      int metablockPtrOffset = structIfs.readu16le() - 0xC000;
      structIfs.seek(metablockPtrOffset);
      
      TBufStream decompressedData;
//      std::cerr << std::hex << metablockPtrOffset << std::endl;
      decompressMetablockData(structIfs, decompressedData);
      
//      std::cerr << i << " " << j << std::endl;
//      if ((i == 0) && (j == 0)) decompressedData.save("test_map3.bin");
      
      decompressedData.seek(0);
      for (int metatileY = 0; metatileY < metablockHInMetatiles; metatileY++) {
        for (int metatileX = 0; metatileX < metablockWInMetatiles; metatileX++) {
          int nextMetatileId = decompressedData.readu8();
  
          int targetX
            = (i * metablockWInPixels) + (metatileX * metatileWInPixels);
          int targetY
            = (j * metablockHInPixels) + (metatileY * metatileHInPixels);
          
          // seek metatile data
          structIfs.seek(metatileBlockOffset +
            (nextMetatileId * metatileSizeInBytes));
          renderMetatile(structIfs, tileGraphics, palette, grp, targetX, targetY);
        }
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Tengai Makyou Ziria map renderer" << std::endl;
    std::cout << "Usage: " << argv[0] << " <infile> <sector> <outfile>"
      << " [options]" << std::endl;
    
    std::cout << "Options: " << std::endl;
    std::cout << "   --old    Enable old map format handling"
      << std::endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  int sectorNum = TStringConversion::stringToInt(std::string(argv[2]));
  std::string outFile(argv[3]);
  
  if (TOpt::hasFlag(argc, argv, "--old")) oldMapMode = true;

  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  int mapSector = sectorNum;
  
  ifs.seek(mapSector * sectorSize);
  TBufStream mapIfs;
  mapIfs.writeFrom(ifs, 0x18 * sectorSize);
  
  mapIfs.seek(0x2000);
  int firstVal = mapIfs.readu16le();
  if (firstVal == 0x0000) {
    std::cout << "skipping blank structure map "
      << TStringConversion::intToString(mapSector,
          TStringConversion::baseHex) << std::endl;
    return 1;
  }
  else if (firstVal == 0xFFFF) {
    std::cout << "skipping 0xFFFF structure map "
      << TStringConversion::intToString(mapSector,
          TStringConversion::baseHex) << std::endl;
    return 1;
  }
  else if (firstVal == 0x00FF) {
    std::cout << "skipping 0x00FF structure map "
      << TStringConversion::intToString(mapSector,
          TStringConversion::baseHex) << std::endl;
    return 1;
  }
  
  mapIfs.seek(0x2000);
  int first = mapIfs.readu16le();
  int second = mapIfs.readu16le();
  int third = mapIfs.readu16le();
  int fourth = mapIfs.readu16le();
  int fifth = mapIfs.readu16le();
  if ((first < 0xC000) || (third < 0xC000)
      || (fourth < 0xC000) || (fifth < 0xC000)
      || (first >= 0xE000) || (third >= 0xE000)
      || (fourth >= 0xE000) || (fifth >= 0xE000)
      
      ) {
    std::cout << "skipping misc invalid structure map "
      << TStringConversion::intToString(mapSector,
          TStringConversion::baseHex) << std::endl;
    return 1;
  }
  
//    mapIfs.save("test_map4.bin");
  
  std::cout << "rendering map "
      << TStringConversion::intToString(mapSector,
          TStringConversion::baseHex) << std::endl;
  
  mapIfs.seek(0);
  TGraphic output;
  renderMap(mapIfs, output);
  
  std::string outName = outFile;
  TPngConversion::graphicToRGBAPng(outName, output);
  
  return 0;
}
