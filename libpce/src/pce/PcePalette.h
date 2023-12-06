#ifndef PCEPALETTE_H
#define PCEPALETTE_H


#include "util/TStream.h"
#include "util/TTwoDArray.h"
#include "util/TByte.h"
#include "util/TColor.h"
#include "util/TGraphic.h"
#include "pce/PceColor.h"
#include "pce/PcePaletteLine.h"

namespace Pce {


class PcePalette {
public:
  const static int numPaletteLines = 16;
  const static int size = 0x200;

  PcePalette(int precision__ = PceColor::nativePrecision,
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
            
  void generatePreviewFile(const char* filename) const;
  
  PcePaletteLine paletteLines[numPaletteLines];
  
protected:
  
};


}


#endif
