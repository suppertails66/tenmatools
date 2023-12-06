#include "util/TGaloisField256Cd.h"

namespace BlackT {


  TGaloisField256Cd::TGaloisField256Cd()
    : TGaloisField256(0x11D, 2) {
    
    generator_ =  multPoly(TGaloisField256Polynomial({1, 1}),
                           TGaloisField256Polynomial({1, 2}));
    
    for (int i = 0; i < generator_.size(); i++) {
      genLogs_[i] = (generator_[i] == 0) ? 255 : logTable[generator_[i]];
    }
  }
    
  TGaloisField256Polynomial TGaloisField256Cd::calcReedSolomonCode(
      const TGaloisField256Polynomial& message,
      int codewordLength) const {
    return TGaloisField256
      ::calcReedSolomonCode(message, generator_, codewordLength, genLogs_);
  }


} 
