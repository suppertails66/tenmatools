#ifndef PCEVRAM_H
#define PCEVRAM_H


#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "util/TArray.h"
#include "pce/PcePalette.h"
#include "pce/PcePattern.h"
#include "pce/PceSpritePattern.h"

namespace Pce {


class PceVram {
public:
  PceVram();
  
  PcePattern getPattern(int patternNum) const;
  PceSpritePattern getSpritePattern(int patternNum) const;
  void setPattern(int patternNum, const PcePattern& src);
  void setSpritePattern(int patternNum, const PceSpritePattern& src);
  void readPatterns(BlackT::TStream& src, int patternNum, int count = -1);
  void readSpritePatterns(BlackT::TStream& src, int patternNum, int count = -1);
  
protected:
  const static int numPatterns = 0x800;
  const static int dataSize = numPatterns * PcePattern::size;
  
  BlackT::TArray<BlackT::TByte> data_;
  
};


}


#endif
