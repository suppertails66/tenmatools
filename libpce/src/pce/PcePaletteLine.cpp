#include "pce/PcePaletteLine.h"
#include <cstring>

using namespace BlackT;

namespace Pce {


PcePaletteLine::PcePaletteLine(int precision__,
            int rshift__,
            int gshift__,
            int bshift__,
            int rmask__,
            int gmask__,
            int bmask__) {
  setProperties(precision__, rshift__, gshift__, bshift__,
                             rmask__, gmask__, bmask__);
}

void PcePaletteLine::read(BlackT::TStream& ifs) {
  for (int i = 0; i < numColors; i++) {
    colors[i].read(ifs);
  }
}

void PcePaletteLine::write(BlackT::TStream& ofs) const {
  for (int i = 0; i < numColors; i++) {
    colors[i].write(ofs);
  }
}

void PcePaletteLine::readRevEnd(BlackT::TStream& ifs) {
  for (int i = 0; i < numColors; i++) {
    colors[i].readRevEnd(ifs);
  }
}

void PcePaletteLine::writeRevEnd(BlackT::TStream& ofs) const {
  for (int i = 0; i < numColors; i++) {
    colors[i].writeRevEnd(ofs);
  }
}

void PcePaletteLine::setProperties(int precision__,
            int rshift__,
            int gshift__,
            int bshift__,
            int rmask__,
            int gmask__,
            int bmask__) {
  for (int i = 0; i < numColors; i++) {
//    colors[i] = PceColor(precision__, rshift__, gshift__, bshift__,
//                                      rmask__, gmask__, bmask__);
    colors[i].precision = precision__;
    colors[i].rshift = rshift__;
    colors[i].gshift = gshift__;
    colors[i].bshift = bshift__;
    colors[i].rmask = rmask__;
    colors[i].gmask = gmask__;
    colors[i].bmask = bmask__;
  }
}
          
void PcePaletteLine::generatePreview(
    BlackT::TGraphic& dst, int xOffset, int yOffset) const {
  for (int k = 0; k < numColors; k++) {
    int x = (xOffset + (k * 16));
    int y = yOffset;
    dst.fillRect(x, y, 16, 16, colors[k].realColor(),
                 TGraphic::noTransUpdate);
  }
}

int PcePaletteLine::matchColor(BlackT::TColor color,
                               bool transparency) const {
  PceColor pceColor;
  pceColor.setRealColor(color);
  
  // do not match color 0, which is reserved for the background,
  // for anything except transparency
  if (transparency && (color.a() == TColor::fullAlphaTransparency))
    return 0;
  
/*          std::cerr << "src: " << (int)color.r() << " " << (int)color.g() << " " << (int)color.b() << " " << std::endl;
          std::cerr << "src: " << (int)pceColor.getNative() << " " << std::endl;
  PceColor test;
  test.fromNative(pceColor.getNative());
          std::cerr << "src: " << (int)test.realColor().r() << " " << (int)test.realColor().g() << " " << (int)test.realColor().b() << std::endl; */
  
  for (int k = (transparency ? 1 : 0); k < numColors; k++) {
//          std::cerr << std::hex << "         " << k << ": " << (int)colors[k].realColor().r() << " " << (int)colors[k].realColor().g() << " " << (int)colors[k].realColor().b() << " " << std::endl;
//          std::cerr << std::hex << "         " << (int)colors[k].getNative() << std::endl;
//          std::cerr << std::hex << "         self: " << (int)pceColor.getNative() << std::endl;
//    if (colors[k] == pceColor) return k;
    if (colors[k] == pceColor) {
//      std::cerr << k << " match" << std::endl;
      return k;
    }
    else {
//      std::cerr << k << " fail" << std::endl;
    }
  }
  
  return -1;
}


}
