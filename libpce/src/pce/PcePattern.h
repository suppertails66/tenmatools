#ifndef PCEPATTERN_H
#define PCEPATTERN_H


#include "util/TStream.h"
#include "util/TTwoDArray.h"
#include "util/TByte.h"
#include "util/TGraphic.h"
#include "pce/PcePaletteLine.h"

namespace Pce {


class PcePattern {
public:
  const static int w = 8;
  const static int h = 8;
  const static int size = 32;

  PcePattern();
  
  bool operator==(const PcePattern& other) const;
  bool operator!=(const PcePattern& other) const;
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  void toGraphic(BlackT::TGraphic& dst,
                 int xOffset = 0, int yOffset = 0,
                 const PcePaletteLine* paletteLine = NULL,
                 bool transparency = false) const;
  
  int fromGraphic(const BlackT::TGraphic& src,
                  int xOffset = 0, int yOffset = 0,
                  const PcePaletteLine* paletteLine = NULL,
                  bool transparency = true);
  
  int getPixel(int x, int y);
  
protected:
  typedef BlackT::TTwoDArray<BlackT::TByte> PatternArray;
  PatternArray data_;
  
};


}


#endif
