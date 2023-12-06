#ifndef TEDCCALC_H
#define TEDCCALC_H


#include "util/TByte.h"
#include <initializer_list>
#include <vector>

namespace BlackT {


  // CD error detection code calculator.
  // this just does a CRC32. (i think. unlike with RS codes there's lots of
  // code available and i didn't get into the details.)
  // implemented per https://problemkaputt.de/psx-spx.htm
  class TEdcCalc {
  public:
    TEdcCalc();
    
    unsigned int calcEdc(const TByte* data, int len) const;
  protected:
    unsigned int edcTable[256];
    
  };


}


#endif
