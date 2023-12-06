#ifndef TSTRINGCONVERSION_H
#define TSTRINGCONVERSION_H


#include <string>
#include <sstream>
#include "util/TStream.h"
#include "util/TByte.h"

namespace BlackT {


class TStringConversion {
public:
  enum NumBaseFormat {
    baseDec,
    baseHex,
    baseOct
  };

  template<class T>
  static T fromString(const std::string& str) {
    std::istringstream iss;
    iss.str(str);
    T result;
    iss >> result;
    return result;
  }
  
  template<class T>
  static std::string toString(const T obj) {
    std::ostringstream oss;
    oss << obj;
    return oss.str();
  }
  
/*  template<class T>
  static int toInt(const T& str,
                   std::ios_base::fmtflags flags
                     = std::ios_base::dec) {
    std::istringstream iss;
    iss.flags(flags);
    iss.str(str);
    int result;
    iss >> result;
    return result;
  } */
  
  static long int stringToInt(const std::string& str) {
    if (str.size() <= 0) {
      return 0;
    }
    
    // convert string to a string to eliminate leading whitespace
    std::string workstr = toString(str);
    
    std::istringstream iss;
    
    // determine correct base
    iss.unsetf(std::ios_base::basefield);
    if (workstr.size() == 1) {
      iss.setf(std::ios_base::dec);
    }
    else {
      if ((workstr[0] == '$') || (workstr[0] == '#')) {
        iss.setf(std::ios_base::hex);
        
        // stdlib doesn't recognize $ prefix for hex, so we have
        // to get rid of it
        workstr = workstr.substr(1, workstr.size() - 1);
      }
      else if (workstr[0] == '0') {
        if (workstr[1] == 'x') {
          iss.setf(std::ios_base::hex);
        }
        else {
          iss.setf(std::ios_base::dec);
        }
      }
      else {
        iss.setf(std::ios_base::dec);
      }
    }
  
    iss.str(workstr);
    long int result;
    iss >> result;
    return result;
  }
  
  static std::string intToString(int val,
                                 NumBaseFormat base
                                   = baseDec) {
    std::ostringstream oss;
    
    oss.setf(std::ios_base::uppercase);
    oss.setf(std::ios_base::showbase);
    oss.unsetf(std::ios_base::basefield);
    switch (base) {
    case baseDec:
    default:
      oss.setf(std::ios_base::dec);
      break;
    case baseHex:
      oss.setf(std::ios_base::hex);
      break;
    case baseOct:
      oss.setf(std::ios_base::oct);
      break;
    }
    
    oss << val;
    
    std::string str = oss.str();
    
    // god damn
    if ((base == baseHex)
        && (oss.flags() & std::ios_base::showbase)) {
      if (str.size() >= 2) {
        str[1] = 'x';
      }
      else {
        str = "0x" + str;
      }
    }
    
    return str;
  }
  
  static std::string intToFixedWHexString(int val, unsigned int width) {
    std::string str = intToString(val, baseHex);
    str = str.substr(2, std::string::npos);
    while (str.size() < width) str = "0" + str;
    return str;
  }
  
  /**
   * Writes N bytes to dst.
   * Content will be filled with str and padded to the required
   * size with bytes of the given value.
   * An error occurs if the input string exceeds the target length.
   *
   * @arg str Source string.
   * @arg dst Destination stream.
   * @arg size Number of output bytes.
   * @arg padval Value to pad with.
   */
  static void writePaddedString(std::string str, BlackT::TStream& dst,
                             int size, BlackT::TByte padval = 0x00);
  
  /**
   * Decapitalizes a string.
   *
   * @arg str Source/destination string.
   */
  static void decapitalize(std::string& str);
protected:
  
};


};


#endif 
