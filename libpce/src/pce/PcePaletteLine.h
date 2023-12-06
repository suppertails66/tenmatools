#ifndef PCEPALETTELINE_H
#define PCEPALETTELINE_H


#include "util/TStream.h"
#include "util/TTwoDArray.h"
#include "util/TByte.h"
#include "util/TColor.h"
#include "util/TGraphic.h"
#include "pce/PceColor.h"

namespace Pce {


class PcePaletteLine {
public:
  const static int numColors = 16;
  const static int size = 32;

  PcePaletteLine(int precision__ = PceColor::nativePrecision,
            int rshift__ = PceColor::nativeRshift,
            int gshift__ = PceColor::nativeGshift,
            int bshift__ = PceColor::nativeBshift,
            int rmask__ = PceColor::nativeRmask,
            int gmask__ = PceColor::nativeGmask,
            int bmask__ = PceColor::nativeBmask);
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  void readRevEnd(BlackT::TStream& ifs);
  void writeRevEnd(BlackT::TStream& ofs) const;
  
  void setProperties(int precision__ = PceColor::nativePrecision,
            int rshift__ = PceColor::nativeRshift,
            int gshift__ = PceColor::nativeGshift,
            int bshift__ = PceColor::nativeBshift,
            int rmask__ = PceColor::nativeRmask,
            int gmask__ = PceColor::nativeGmask,
            int bmask__ = PceColor::nativeBmask);
            
  void generatePreview(BlackT::TGraphic& dst, int xOffset, int yOffset) const;
  
  int matchColor(BlackT::TColor color, bool transparency = true) const;
  
  PceColor colors[numColors];
  
protected:
  
};


}


#endif
