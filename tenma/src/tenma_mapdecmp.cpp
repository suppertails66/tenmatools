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

void decmp(TStream& ifs, TStream& ofs, int outputSize) {
  unsigned char buffer[256];
  std::memset(buffer, 0x00, sizeof(buffer));
  unsigned char bufferPos = 0xEF;
  
  int remaining = outputSize;
  unsigned char cmd = 0;
  unsigned char compressionNybbles = 0;
  bool compressionNybblesSecond = true;
  int bitCounter = 0;
  while (remaining > 0) {
    --bitCounter;
    if (bitCounter < 0) {
      cmd = ifs.get();
      bitCounter = 7;
    }
    
    bool isLiteral = ((cmd & 0x80) != 0);
    cmd <<= 1;
    
    if (isLiteral) {
      unsigned char value = ifs.get();
      buffer[bufferPos++] = value;
      ofs.put(value);
      --remaining;
    }
    else {
      unsigned char bufferGetPos = ifs.get();
      
      if (compressionNybblesSecond) {
        compressionNybbles = ifs.get();
      }
      else {
        compressionNybbles <<= 4;
      }
      
      unsigned char runLen = ((compressionNybbles & 0xF0) >> 4) + 2;
      compressionNybblesSecond = !compressionNybblesSecond;
      
      for (int i = 0; i < runLen; i++) {
        unsigned char value = buffer[bufferGetPos++];
        ofs.put(value);
        buffer[bufferPos++] = value;
        --remaining;
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Tengai Makyou Ziria map decompressor" << std::endl;
    std::cout << "Usage: " << argv[0] << " <infile> <outfile>" << std::endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string outFile(argv[2]);
  
  std::vector<int> mapIndex = TenmaMsgConsts::getMapSectorIndex();

  TBufStream ifs;
  ifs.open(inFile.c_str());
  
//  for (auto mapSector: mapIndex) {
  for (int i = 0; i < mapIndex.size(); i++) {
//  for (int i = 161; i < mapIndex.size(); i++) {
//  for (int i = 658; i < mapIndex.size(); i++) {
//  for (int i = 667; i < mapIndex.size(); i++) {
//  for (int i = 0; i < 1; i++) {
    int mapSector = mapIndex[i];
    
    int nextMapSector = mapSector + 0x1A;
    if (i != (mapIndex.size() - 1)) {
      nextMapSector = mapIndex[i + 1];
    }
    
    int mapSize = nextMapSector - mapSector;
    if (mapSize < 0x1A) {
      
      std::cout << "skipping small map "
        << i << ": "
        << TStringConversion::intToString(mapSector,
            TStringConversion::baseHex) << std::endl;
      continue;
    }
    
    ifs.seek(mapSector * sectorSize);
    TBufStream mapIfs;
//    mapIfs.writeFrom(ifs, 0x18 * sectorSize);
    mapIfs.writeFrom(ifs, 0x10 * sectorSize);
    
/*    mapIfs.seek(0x2000);
    int firstVal = mapIfs.readu16le();
    if (firstVal == 0x0000) {
      std::cout << "skipping blank structure map "
        << i << ": "
        << TStringConversion::intToString(mapSector,
            TStringConversion::baseHex) << std::endl;
      continue;
    }
    else if (firstVal == 0xFFFF) {
      std::cout << "skipping 0xFFFF structure map "
        << i << ": "
        << TStringConversion::intToString(mapSector,
            TStringConversion::baseHex) << std::endl;
      continue;
    }
    else if (firstVal == 0x00FF) {
      std::cout << "skipping 0x00FF structure map "
        << i << ": "
        << TStringConversion::intToString(mapSector,
            TStringConversion::baseHex) << std::endl;
      continue;
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
        << i << ": "
        << TStringConversion::intToString(mapSector,
            TStringConversion::baseHex) << std::endl;
      continue;
    }*/
    
//    mapIfs.save("test_map4.bin");

    // just to make things painful, it appears that unused maps were not
    // compressed
    mapIfs.seek(0);
    bool compressed = false;
    for (int j = 0; j < 4; j++) {
      int ptr = mapIfs.readu16le();
      
      if ((ptr < 0xC000) || (ptr >= 0xE000)) {
        compressed = true;
        break;
      }
    }
    
    if (!compressed) {
      std::cout << "skipping apparently uncompressed map "
          << i << ": "
          << TStringConversion::intToString(mapSector,
              TStringConversion::baseHex) << std::endl;
        continue;
    }
    
    std::cout << "decompressing map "
      << i << ": "
        << TStringConversion::intToString(mapSector,
            TStringConversion::baseHex) << std::endl;
    
    mapIfs.seek(0);
    ifs.seek(mapSector * sectorSize);
    decmp(mapIfs, ifs, 0xD000);
    
/*    std::string outName = outPrefix + "map_"
      + TStringConversion::intToString(i)
      + "-"
      + TStringConversion::intToString(mapSector,
          TStringConversion::baseHex)
      + ".png";
    TPngConversion::graphicToRGBAPng(outName, output);*/
  }
  
  ifs.save(outFile.c_str());
  
  return 0;
}
