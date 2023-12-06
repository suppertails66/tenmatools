#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <cctype>

namespace BlackT {


  void TStringConversion::writePaddedString(std::string str, BlackT::TStream& dst,
                             int size, BlackT::TByte padval) {
    if (str.size() > size) {
      throw TGenericException(T_SRCANDLINE,
                              "writePaddedString()",
                              std::string("String \"")
                                + str
                                + "\""
                                + " too long to write: limit is "
                                + TStringConversion::toString(size));
    }
    
    dst.write(str.c_str(), str.size());
    
    int remaining = size - str.size();
    for (int i = 0; i < remaining; i++) {
      dst.put(padval);
    }
  }
  
  void TStringConversion::decapitalize(std::string& str) {
    for (unsigned int i = 0; i < str.size(); i++) str[i] = tolower(str[i]);
  }


}; 
