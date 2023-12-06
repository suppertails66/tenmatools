#ifndef PCECOLOR_H
#define PCECOLOR_H


#include "util/TStream.h"
#include "util/TTwoDArray.h"
#include "util/TByte.h"
#include "util/TColor.h"

namespace Pce {


class PceColor {
public:
  const static int nativePrecision = 3;
  const static int nativeRshift = 3;
  const static int nativeGshift = 6;
  const static int nativeBshift = 0;
  const static int nativeRmask = 0x0038;
  const static int nativeGmask = 0x01C0;
  const static int nativeBmask = 0x0007;
  
  const static int realColorRange = 0xFF;

  PceColor(int precision__ = nativePrecision,
            int rshift__ = nativeRshift,
            int gshift__ = nativeGshift,
            int bshift__ = nativeBshift,
            int rmask__ = nativeRmask,
            int gmask__ = nativeGmask,
            int bmask__ = nativeBmask);
  
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
  
  BlackT::TColor realColor() const;
  void setRealColor(BlackT::TColor color__);
  
  void fromNative(int value);
  int getNative() const;
  
  bool operator==(const PceColor& other) const;
  bool operator!=(const PceColor& other) const;
  
  int precision;
  int rshift;
  int gshift;
  int bshift;
  int rmask;
  int gmask;
  int bmask;
  
protected:
  BlackT::TColor color_;
  
};


}


#endif
