#include "pce/PceTileId.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"

using namespace BlackT;

namespace Pce {


PceTileId::PceTileId()
  : palette(0),
    pattern(0) { }

void PceTileId::read(const char* data) {
  int raw = ByteConversion::fromBytes(data, 2,
    EndiannessTypes::little, SignednessTypes::nosign);
  palette = (raw & 0xF000) >> 12;
  pattern = (raw & 0x0FFF);
}

void PceTileId::write(char* data) const {
  int raw = 0;
  raw |= (palette << 12);
  raw |= pattern;
  
  ByteConversion::toBytes(raw, data, 2,
    EndiannessTypes::little, SignednessTypes::nosign);
}

void PceTileId::read(BlackT::TStream& ifs) {
  int raw = ifs.readu16le();
  palette = (raw & 0xF000) >> 12;
  pattern = (raw & 0x0FFF);
}

void PceTileId::write(BlackT::TStream& ofs) const {
  int raw = 0;
  raw |= (palette << 12);
  raw |= pattern;
  
  ofs.writeu16le(raw);
}
  
/*void PceTileId::toColorGraphic(TGraphic& dst,
                    const MdVram& vram,
                    const MdPalette& pal,
                    int x, int y) const {
  const MdPattern& pat = vram.getPattern(pattern);
  
  TGraphic temp(MdPattern::w, MdPattern::h);
  pat.toColorGraphic(temp, pal.lines[palette], 0, 0);
  
  if (vflip) temp.flipVertical();
  if (hflip) temp.flipHorizontal();
  
  dst.copy(temp,
           TRect(x, y, 0, 0),
           TRect(0, 0, 0, 0));
}
  
void PceTileId::toGrayscaleGraphic(TGraphic& dst,
                    const MdVram& vram,
                    int x, int y) const {
  const MdPattern& pat = vram.getPattern(pattern);
  
  TGraphic temp(MdPattern::w, MdPattern::h);
  temp.clearTransparent();
  pat.toGrayscaleGraphic(temp, 0, 0);
  
  if (vflip) temp.flipVertical();
  if (hflip) temp.flipHorizontal();
  
  dst.copy(temp,
           TRect(x, y, 0, 0),
           TRect(0, 0, 0, 0));
} */


}
