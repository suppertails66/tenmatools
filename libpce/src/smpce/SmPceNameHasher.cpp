#include "smpce/SmPceNameHasher.h"
#include "smpce/SmPceScriptOps.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


std::string SmPceNameHasher::hash(const std::string& name) {
  /*
    Hash format:

      byte 0:
        (((name[0] & 0x0F) << 4) | (name[1] & 0x0F))
      byte 1:
        hashbyte = 0xFF
        if (name has a '.' in it, at position X)
          hashbyte = (((name[X - 2] & 0x0F) << 4) | (name[X - 1] & 0x0F))
        if (hashbyte == 0xFF)
          hashbyte = (((name[size - 2] & 0x0F) << 4) | (name[size - 1] & 0x0F))
      byte 2:
        8-bit sum of all characters in string, including '.' if present,
        with all lower case letters converted to upper case
  
  */
  
  if (name.size() < 2) return "";
  
  unsigned char byte0 = 0x00;
  unsigned char byte1 = 0xFF;
  unsigned char byte2 = 0x00;
  
  byte0 = ((name[0] & 0x0F) << 4) | (name[1] & 0x0F);
  
  for (int i = 2; i < name.size(); i++) {
    if (name[i] == '.') {
      byte1 = ((name[i - 2] & 0x0F) << 4) | (name[i - 1] & 0x0F);
      break;
    }
  }
  if (byte1 == 0xFF) {
    byte1 = ((name[name.size() - 2] & 0x0F) << 4)
              | (name[name.size() - 1] & 0x0F);
  }
  
  for (int i = 0; i < name.size(); i++) {
    byte2 += toupper((unsigned char)name[i]);
  }
  
  int result = 0;
  
  result |= byte0;
  result <<= 8;
  result |= byte1;
  result <<= 8;
  result |= byte2;
  
  std::string resultStr
    = TStringConversion::intToString(result,
         TStringConversion::baseHex).substr(2, std::string::npos);
  while (resultStr.size() < 6) resultStr = std::string("0") + resultStr;
  
  return resultStr;
}


}
