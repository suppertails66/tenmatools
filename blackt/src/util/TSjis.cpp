#include "util/TSjis.h"

namespace BlackT {


bool TSjis::isSjis(int code) {
  // null == terminator
  if (code == 0) return false;
  
  int highbyte = (code & 0xFF00) >> 8;
//  int lowbyte = (code & 0x00FF);
  
  // two-byte sequence?
  if (between(highbyte, 0x81, 0x9F)
      || between(highbyte, 0xE0, 0xEF)) {
    return (is2bSjis(code));
  }
  
  // one-byte sequence?
  if ((code == 0x0A)    // cr
      || (code == 0x0D) // lf
      || between(code, 0x20, 0x7F)
      || between(code, 0xA1, 0xDF)) return true;
  
  // invalid
  return false;
}

int TSjis::checkSjisChar(TStream& ifs) {
  int origPos = ifs.tell();
  
  unsigned char next = ifs.get();
  
  // null == terminator
  if (next == 0) {
    ifs.seek(origPos);
    return -1;
  }
  
  // two-byte sequence?
  if (between(next, 0x81, 0x9F)
      || between(next, 0xE0, 0xEF)) {
    if (ifs.remaining() < 1) return -1;
    unsigned char nextnext = ifs.get();
    ifs.seek(origPos);
    int code = nextnext;
    code |= ((int)next << 8);
    
    if (is2bSjis(code)) return 2;
    else return -1;
  }
  
  ifs.seek(origPos);
  
  // one-byte sequence?
  if ((next == 0x0A)    // cr
      || (next == 0x0D) // lf
      || between(next, 0x20, 0x7F)
      || between(next, 0xA1, 0xDF)) return 1;
  
  // invalid
  return -1;
}

int TSjis::fetchSjisChar(TStream& ifs) {
  int sz = checkSjisChar(ifs);
  if (sz <= 0) return -1;
  
  ifs.seekoff(-sz);
  
  int result = 0;
  for (int i = 0; i < sz; i++) {
    result <<= 8;
    result |= ifs.readu8();
  }
  
  return result;
}

TSjis::SjisCheckResult TSjis::checkSjisString(TStream& ifs) {
  int start = ifs.tell();
  
  SjisCheckResult result;
  result.rawLength = -1;
  result.charCount = -1;
  
  int len = 0;
  int charCount = 0;
  
  // assume no intermixed full/halfwidth sequences
//  bool halfwidth = false;
//  if (between(buffer[pos], 0xA0, 0xDF)) {
//    halfwidth = true;
//  }
  
  bool terminated = false;
  while (!ifs.eof()) {
    if (ifs.peek() == 0) {
      terminated = true;
      break;
    }
    
    int nextCharSize = checkSjisChar(ifs);
    if (nextCharSize <= 0) break;
    
    len += nextCharSize;
    ++charCount;
  }
  
  // rewind stream to initial position
  ifs.seek(start);
  ifs.clear();
  
  // if the null string, or no terminator found
  if (!terminated || (len == 0) || ifs.eof()) return result;
  
  result.rawLength = len + 1; // include terminator in length count
  result.charCount = charCount;
  return result;
}

bool TSjis::between(int value, int lower, int upper) {
  return (((value < lower) || (value > upper)) ? false : true);
}

bool TSjis::is2bSjis(int code) {
  
  int lowbyte = code & 0x00FF;
  if ((lowbyte < 0x40)
      || (lowbyte == 0x7F)
      || (lowbyte >= 0xFD)) return false;
  
  int highbyte = (code & 0xFF00) >> 8;
  if ((highbyte < 0x81)) return false;

  // oh boy
  if (between(code, 0x81AD, 0x81B7)
      || between(code, 0x81C0, 0x81C7)
      || between(code, 0x81CF, 0x81D9)
      || between(code, 0x81E9, 0x81EF)
      || between(code, 0x81F8, 0x81FB)
      || between(code, 0x8240, 0x824E)
      || between(code, 0x8259, 0x825F)
      || between(code, 0x827A, 0x8280)
      || between(code, 0x829B, 0x829E)
      || between(code, 0x82F2, 0x82FC)
      || between(code, 0x8397, 0x839E)
      || between(code, 0x83B7, 0x83BE)
      || between(code, 0x83D7, 0x83FC)
      || between(code, 0x8461, 0x846F)
      || between(code, 0x8492, 0x849E)
      || between(code, 0x84BF, 0x86FC)
      || (code == 0x875E)
      || between(code, 0x8776, 0x877D)
      || between(code, 0x879D, 0x889E)
      
      || between(code, 0xA040, 0xE03F)
      
      || between(code, 0xEAA5, 0xECFC)
      || between(code, 0xEEED, 0xEEEE)
      || between(code, 0xEF40, 0xFFFF)
      
      // unsupported extensions
      || between(code, 0x84BF, 0x889E)
      || between(code, 0x9873, 0x989E)
      || between(code, 0xEAA5, 0xFFFF)
      
      ) return false;
  
  return true;
}


};
