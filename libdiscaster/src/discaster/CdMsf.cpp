#include "discaster/CdMsf.h"
#include "util/MiscMath.h"

namespace Discaster {


  CdMsf::CdMsf()
    : min(0),
      sec(0),
      frame(0) { };
  
  void CdMsf::fromSectorNum(int sectorNum) {
    frame = sectorNum % 75;
    sectorNum /= 75;
    sec = sectorNum % 60;
    min = sectorNum / 60;
  }
  
  int CdMsf::toSectorNum() const {
    return ((min * (60 * 75)) + (sec * 75) + frame);
  }
  
  void CdMsf::fromSectorNumToBcd(int sectorNum) {
    fromSectorNum(sectorNum);
    
    min = BlackT::MiscMath::toBcd(min);
    sec = BlackT::MiscMath::toBcd(sec);
    frame = BlackT::MiscMath::toBcd(frame);
  }


}
