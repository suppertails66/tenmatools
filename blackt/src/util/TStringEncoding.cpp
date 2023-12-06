#include "util/TStringEncoding.h"
#include <cstdlib>

namespace BlackT {


int TStringEncoding::readSjisString(const char* src, std::string& dst) {
  while (*src != 0) {
    char next = *(src++);
    dst += next;
    // if first byte is 0x80 or more, this is a 2-byte sequence
    if (next < 0) dst += *(src++);
  }
  
  // length of string content + null terminator
  return dst.size() + 1;
}


} 
