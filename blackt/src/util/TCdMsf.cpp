#include "util/TCdMsf.h"
#include "util/MiscMath.h"

namespace BlackT {


  TCdMsf::TCdMsf()
    : min(0),
      sec(0),
      frame(0) { };
  
  void TCdMsf::fromSectorNum(int sectorNum) {
    frame = sectorNum % 75;
    sectorNum /= 75;
    sec = sectorNum % 60;
    min = sectorNum / 60;
  }
  
  int TCdMsf::toSectorNum() const {
    return ((min * (60 * 75)) + (sec * 75) + frame);
  }
  
  void TCdMsf::fromSectorNumToBcd(int sectorNum) {
    fromSectorNum(sectorNum);
    
    min = BlackT::MiscMath::toBcd(min);
    sec = BlackT::MiscMath::toBcd(sec);
    frame = BlackT::MiscMath::toBcd(frame);
  }


}
