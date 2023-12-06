#include "pce/PcePalette.h"
#include "util/TPngConversion.h"
#include <cstring>

using namespace BlackT;

namespace Pce {


PcePalette::PcePalette(int precision__,
            int rshift__,
            int gshift__,
            int bshift__,
            int rmask__,
            int gmask__,
            int bmask__) {
  setProperties(precision__, rshift__, gshift__, bshift__,
                             rmask__, gmask__, bmask__);
}

void PcePalette::read(BlackT::TStream& ifs) {
  for (int i = 0; i < numPaletteLines; i++) {
    paletteLines[i].read(ifs);
  }
}

void PcePalette::write(BlackT::TStream& ofs) const {
  for (int i = 0; i < numPaletteLines; i++) {
    paletteLines[i].write(ofs);
  }
}

void PcePalette::readRevEnd(BlackT::TStream& ifs) {
  for (int i = 0; i < numPaletteLines; i++) {
    paletteLines[i].readRevEnd(ifs);
  }
}

void PcePalette::writeRevEnd(BlackT::TStream& ofs) const {
  for (int i = 0; i < numPaletteLines; i++) {
    paletteLines[i].writeRevEnd(ofs);
  }
}

void PcePalette::setProperties(int precision__,
            int rshift__,
            int gshift__,
            int bshift__,
            int rmask__,
            int gmask__,
            int bmask__) {
  for (int i = 0; i < numPaletteLines; i++) {
    paletteLines[i].setProperties(precision__, rshift__, gshift__, bshift__,
                                      rmask__, gmask__, bmask__);
  }
}
          
void PcePalette::generatePreviewFile(const char* filename) const {
  TGraphic g(16 * 16, 16 * 16);
  g.clearTransparent();
  for (int k = 0; k < numPaletteLines; k++) {
    paletteLines[k].generatePreview(g, 0, k * 16);
  }
  TPngConversion::graphicToRGBAPng(std::string(filename), g);
}


}
