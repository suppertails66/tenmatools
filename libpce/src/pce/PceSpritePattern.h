#ifndef PCESPRITEPATTERN_H
#define PCESPRITEPATTERN_H


#include "util/TStream.h"
#include "util/TTwoDArray.h"
#include "util/TByte.h"
#include "util/TGraphic.h"
#include "pce/PcePaletteLine.h"

namespace Pce {


class PceSpritePattern {
public:
  const static int w = 16;
  const static int h = 16;
  const static int size = 128;
  const static int wordsPerBitplane = 16;
  const static int wordsPerPattern = wordsPerBitplane * 4;

  PceSpritePattern();
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  void toGraphic(BlackT::TGraphic& dst,
                 int xOffset = 0, int yOffset = 0,
                 PcePaletteLine* paletteLine = NULL,
                 bool transparency = false);
  
  int fromGraphic(const BlackT::TGraphic& src,
                  int xOffset = 0, int yOffset = 0,
                  const PcePaletteLine* paletteLine = NULL);
  
protected:
  typedef BlackT::TTwoDArray<BlackT::TByte> PatternArray;
  PatternArray data_;
  
};


}


#endif
