#ifndef TGALOISFIELD256CD_H
#define TGALOISFIELD256CD_H


#include "util/TGaloisField256.h"
#include "util/TByte.h"
#include <initializer_list>
#include <vector>
#include <cstdlib>

namespace BlackT {


  class TGaloisField256Cd : public TGaloisField256 {
  public:
    TGaloisField256Cd();
    
    TGaloisField256Polynomial calcReedSolomonCode(
      const TGaloisField256Polynomial& message,
      int codewordLength) const;
  protected:
    TGaloisField256Polynomial generator_;
    TByte genLogs_[256];
  };


}


#endif
