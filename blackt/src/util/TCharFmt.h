#ifndef TCHARFMT_H
#define TCHARFMT_H


#include <vector>
#include <string>

namespace BlackT {


//typedef short short int TChar8;
typedef short int TChar16;
typedef int TChar32;

typedef std::vector<TChar16> TUtf16Chars;
typedef std::vector<TChar32> TUtf32Chars;

class TCharFmt {
public:
  
  static void utf8To16(const std::string& src,
                TUtf16Chars& dst);
  
  static void utf8To32(const std::string& src,
                TUtf32Chars& dst);
  
protected:
  
};


}


#endif
