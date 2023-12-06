#include "util/TEdcCalc.h"
#include <iostream>

namespace BlackT {

  TEdcCalc::TEdcCalc() {
    // initialize table
    for (unsigned int i = 0; i < 256; i++) {
      unsigned int x = i;
      for (unsigned int j = 0; j < 8; j++) {
        bool shiftOut = ((x & 0x01) != 0);
        x >>= 1;
        if (shiftOut) x ^= 0xD8018001;
      }
      edcTable[i] = x;
    }
    
//    std::cerr << std::hex << edcTable[1] << std::endl;
  }
  
  unsigned int TEdcCalc::calcEdc(const TByte* data, int len) const {
    unsigned int x = 0;
    for (int i = 0; i < len; i++) {
//      unsigned int index = (x ^ *(data + i)) & 0xFF;
//      x = (x >> 8) ^ edcTable[index];
      x ^= *(data + i);
      x = (x >> 8) ^ edcTable[x & 0xFF];
    }
    return x;
  }


} 
