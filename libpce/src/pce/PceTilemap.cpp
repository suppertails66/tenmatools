#include "pce/PceTilemap.h"

using namespace BlackT;

namespace Pce {


PceTilemap::PceTilemap() { }
  
void PceTilemap::resize(int w, int h) {
  tileIds.resize(w, h);
}

const PceTileId& PceTilemap::getTileId(int x, int y) const {
  return tileIds.data(x, y);
}

void PceTilemap::setTileId(int x, int y, const PceTileId& tileId) {
  tileIds.data(x, y) = tileId;
}

void PceTilemap::read(const char* src, int w, int h) {
  resize(w, h);
  
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      tileIds.data(i, j).read(src);
      src += PceTileId::size;
    }
  }
}

void PceTilemap::read(BlackT::TStream& ifs, int w, int h) {
  resize(w, h);
  
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      tileIds.data(i, j).read(ifs);
    }
  }
}

void PceTilemap::write(BlackT::TStream& ofs) const {
  for (int j = 0; j < tileIds.h(); j++) {
    for (int i = 0; i < tileIds.w(); i++) {
      tileIds.data(i, j).write(ofs);
    }
  }
}

void PceTilemap::toGraphic(BlackT::TGraphic& dst,
               const PceVram& vram,
               const PcePalette* pal,
               bool transparency) const {
  dst.resize(tileIds.w() * PcePattern::w,
             tileIds.h() * PcePattern::h);
  
  for (int j = 0; j < tileIds.h(); j++) {
    for (int i = 0; i < tileIds.w(); i++) {
      int xOffset = (i * PcePattern::w);
      int yOffset = (j * PcePattern::h);
      
      const PceTileId& tileId = tileIds.data(i, j);
      const PcePaletteLine* palLine = NULL;
      
      if (pal != NULL) {
        palLine = &(pal->paletteLines[tileId.palette]);
      }
      
      PcePattern pattern = vram.getPattern(tileId.pattern);
      pattern.toGraphic(dst, xOffset, yOffset, palLine, transparency);
    }
  }
}
  
/*void PceTilemap::toColorGraphic(BlackT::TGraphic& dst,
                    const MdVram& vram,
                    const MdPalette& pal) {
  dst.resize(tileIds.w() * MdPattern::w,
             tileIds.h() * MdPattern::h);
  dst.clearTransparent();
  
  for (int j = 0; j < tileIds.h(); j++) {
    for (int i = 0; i < tileIds.w(); i++) {
      tileIds.data(i, j).toColorGraphic(
        dst, vram, pal, (i * MdPattern::w), (j * MdPattern::h));
    }
  }
}
  
void PceTilemap::toGrayscaleGraphic(BlackT::TGraphic& dst,
                    const MdVram& vram) {
  dst.resize(tileIds.w() * MdPattern::w,
             tileIds.h() * MdPattern::h);
  dst.clearTransparent();
  
  for (int j = 0; j < tileIds.h(); j++) {
    for (int i = 0; i < tileIds.w(); i++) {
      tileIds.data(i, j).toGrayscaleGraphic(
        dst, vram, (i * MdPattern::w), (j * MdPattern::h));
    }
  }
} */


}
