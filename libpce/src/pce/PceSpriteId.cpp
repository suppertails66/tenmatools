#include "pce/PceSpriteId.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"

using namespace BlackT;

namespace Pce {


PceSpriteId::PceSpriteId()
  : y(0),
    x(0),
    pattern(0),
    cgMode(false),
    vFlip(false),
    height(0),
    hFlip(false),
    width(0),
    priority(false),
    palette(0) { }

/*void PceSpriteId::read(const char* data) {
  int raw = ByteConversion::fromBytes(data, 2,
    EndiannessTypes::little, SignednessTypes::nosign);
  palette = (raw & 0xF000) >> 12;
  pattern = (raw & 0x0FFF);
}

void PceSpriteId::write(char* data) const {
  int raw = 0;
  raw |= (palette << 12);
  raw |= pattern;
  
  ByteConversion::toBytes(raw, data, 2,
    EndiannessTypes::little, SignednessTypes::nosign);
} */

void PceSpriteId::read(BlackT::TStream& ifs) {
  y = ifs.readu16le() & 0x3FF;
  x = ifs.readu16le() & 0x3FF;
  
  int patternWord = ifs.readu16le();
  pattern = (patternWord & 0x7FE) >> 1;
  cgMode = (patternWord & 0x1) != 0;
  
  int flags = ifs.readu16le();
  vFlip = (flags & 0x8000) != 0;
  height = (flags & 0x3000) >> 12;
  hFlip = (flags & 0x0800) != 0;
  width = (flags & 0x0100) >> 8;
  priority = (flags & 0x0080) != 0;
  palette = (flags & 0x000F);
}

void PceSpriteId::write(BlackT::TStream& ofs) const {
  ofs.writeu16le(y);
  ofs.writeu16le(x);
  
  int patternWord = 0;
  patternWord |= (pattern << 1);
  if (cgMode) patternWord |= 0x1;
  ofs.writeu16le(patternWord);
  
  int flags = 0;
  if (vFlip) flags |= 0x8000;
  flags |= (height << 12);
  if (hFlip) flags |= 0x0800;
  flags |= (width << 8);
  if (priority) flags |= 0x0080;
  flags |= (palette);
  ofs.writeu16le(flags);
}


}
