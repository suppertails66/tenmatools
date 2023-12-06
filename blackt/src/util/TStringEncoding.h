#ifndef TSTRINGENCODING_H
#define TSTRINGENCODING_H


#include <string>

namespace BlackT {


class TStringEncoding {
public:
  
  static int readSjisString(const char* src, std::string& dst);
  
protected:
  
};


}


#endif
