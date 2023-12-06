#include "pce/PceColor.h"
#include <cstring>
#include <cmath>
#include <iostream>

using namespace BlackT;

namespace Pce {


PceColor::PceColor(int precision__,
            int rshift__,
            int gshift__,
            int bshift__,
            int rmask__,
            int gmask__,
            int bmask__)
  : precision(precision__),
    rshift(rshift__),
    gshift(gshift__),
    bshift(bshift__),
    rmask(rmask__),
    gmask(gmask__),
    bmask(bmask__) { }

void PceColor::read(BlackT::TStream& ifs) {
  int value = ifs.readu16le();
  fromNative(value);
//  std::cerr << std::hex << value << " " << std::hex << getNative() << std::endl;
//  char c;
//  std::cin >> c;
}

void PceColor::write(BlackT::TStream& ofs) const {
  ofs.writeu16le(getNative());
}

void PceColor::readRevEnd(BlackT::TStream& ifs) {
  int value = ifs.readu16be();
  fromNative(value);
}

void PceColor::writeRevEnd(BlackT::TStream& ofs) const {
  ofs.writeu16be(getNative());
}

void PceColor::setProperties(int precision__,
            int rshift__,
            int gshift__,
            int bshift__,
            int rmask__,
            int gmask__,
            int bmask__) {
  precision = precision__;
  rshift = rshift__;
  gshift = gshift__;
  bshift = bshift__;
  rmask = rmask__;
  gmask = gmask__;
  bmask = bmask__;
}

BlackT::TColor PceColor::realColor() const {
  return color_;
}

void PceColor::setRealColor(BlackT::TColor color__) {
//  color_ = color__;
  color_ = color__;
  fromNative(getNative());
}

void PceColor::fromNative(int value) {
  int r = (value & rmask) >> rshift;
  int g = (value & gmask) >> gshift;
  int b = (value & bmask) >> bshift;
  
//  std::cerr << std::hex << r << " " << std::hex << g << " " << std::hex << b << std::endl;
  
  double range = ((int)1 << precision) - 1;
  int realR = ((double)r / range) * realColorRange;
  int realG = ((double)g / range) * realColorRange;
  int realB = ((double)b / range) * realColorRange;
  
//  std::cerr << std::hex << realR << " " << std::hex << realG << " " << std::hex << realB << std::endl;
  
  color_ = TColor(realR, realG, realB, TColor::fullAlphaOpacity);
}

int PceColor::getNative() const {
  int realR = color_.r();
  int realG = color_.g();
  int realB = color_.b();
  
  int range = ((int)1 << precision) - 1;
  int r = std::round(((double)realR / (double)realColorRange) * (double)range);
  int g = std::round(((double)realG / (double)realColorRange) * (double)range);
  int b = std::round(((double)realB / (double)realColorRange) * (double)range);
  
  r <<= rshift;
  r &= rmask;
  g <<= gshift;
  g &= gmask;
  b <<= bshift;
  b &= bmask;
  
  return (r | g | b);
}

bool PceColor::operator==(const PceColor& other) const {
  return (getNative() == other.getNative());
}

bool PceColor::operator!=(const PceColor& other) const {
  return !(getNative() == other.getNative());
}


}
