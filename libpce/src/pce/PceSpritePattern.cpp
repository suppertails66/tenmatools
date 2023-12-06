#include "pce/PceSpritePattern.h"
#include <cstring>

using namespace BlackT;

namespace Pce {


PceSpritePattern::PceSpritePattern() {
  data_.resize(w, h);
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      data_.data(i, j) = 0;
    }
  }
}

void PceSpritePattern::read(TStream& ifs) {
/*  TByte buffer[size];
  ifs.read((char*)buffer, size);
  
  for (int j = 0; j < h; j++) {
    TByte mask = 0x80;
    int basePos = (j * 2);
    for (int i = 0; i < w; i++) {
      TByte result = 0;
      
      if ((buffer[basePos + 0] & mask) != 0)  result |= 0x01;
      if ((buffer[basePos + 16] & mask) != 0)  result |= 0x02;
      if ((buffer[basePos + 32] & mask) != 0) result |= 0x04;
      if ((buffer[basePos + 48] & mask) != 0) result |= 0x08;
      
      data_.data(i, j) = result;
      
      mask >>= 1;
    }
  } */
  
  // clear existing data
  data_.fill(0);
  
  // read 4 bitplanes
  int setmask = 0x01;
  for (int i = 0; i < 4; i++) {
    
    // read 16 words in plane for 16 lines in pattern
    for (int j = 0; j < 16; j++) {
      int next = ifs.readu16le();
      
      int getmask = 0x8000;
      for (int k = 0; k < 16; k++) {
        if ((next & getmask) != 0) {
          data_.data(k, j) |= setmask;
        }
        
        getmask >>= 1;
      }
    }
    
    setmask <<= 1;
  }
}

void PceSpritePattern::write(TStream& ofs) const {
/*  TByte buffer[size];
  std::memset(buffer, 0, size);
  
  for (int j = 0; j < h; j++) {
    char mask = 0x80;
//    int shift = 7;
    int basePos = (j * 2);
    for (int i = 0; i < w; i++) {
      TByte src = data_.data(i, j);
      
      if ((src & 0x01) != 0) buffer[basePos + 0]  |= mask;
      if ((src & 0x02) != 0) buffer[basePos + 1]  |= mask;
      if ((src & 0x04) != 0) buffer[basePos + 16] |= mask;
      if ((src & 0x08) != 0) buffer[basePos + 17] |= mask;
      
      mask >>= 1;
//      --shift;
    }
  }
  
  ofs.write((char*)buffer, size); */
  
  // inefficient conversion via full ints because i don't feel like
  // wasting time on something optimal i don't need
  int buffer[wordsPerPattern];
  std::memset(buffer, 0, sizeof(int)*wordsPerPattern);
  for (int j = 0; j < h; j++) {
    int mask = 0x8000;
    for (int i = 0; i < w; i++) {
      TByte src = data_.data(i, j);
      
      if ((src & 0x01) != 0) buffer[(wordsPerBitplane * 0) + j] |= mask;
      if ((src & 0x02) != 0) buffer[(wordsPerBitplane * 1) + j] |= mask;
      if ((src & 0x04) != 0) buffer[(wordsPerBitplane * 2) + j] |= mask;
      if ((src & 0x08) != 0) buffer[(wordsPerBitplane * 3) + j] |= mask;
      
      mask >>= 1;
    }
  }
  
  for (int i = 0; i < wordsPerPattern; i++) {
    ofs.writeu16le(buffer[i]);
  }
}

void PceSpritePattern::toGraphic(BlackT::TGraphic& dst,
               int xOffset, int yOffset,
               PcePaletteLine* paletteLine,
               bool transparency) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int x = xOffset + i;
      int y = yOffset + j;
    
      int index = data_.data(i, j);
      
      TColor color;
      if (paletteLine == NULL) {
        int component = (index << 4) | index;
        color = TColor(component, component, component,
                       TColor::fullAlphaOpacity);
      }
      else {
        color = paletteLine->colors[index].realColor();
      }
      
      if (transparency && (index == 0)) {
//        color.setA(TColor::fullAlphaTransparency);
      }
      else {
        dst.setPixel(x, y, color);
      }
    }
  }
}

int PceSpritePattern::fromGraphic(const BlackT::TGraphic& src,
                            int xOffset, int yOffset,
                            const PcePaletteLine* paletteLine) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
//      std::cerr << i << " " << j << std::endl;
      
      int x = xOffset + i;
      int y = yOffset + j;
      
      TColor color = src.getPixel(x, y);
//      std::cerr << (unsigned int)color.r() << " " << (unsigned int)color.g() << " " << (unsigned int)color.b() << " " << (unsigned int)color.a() << std::endl;
      int value = -1;
      
      // no palette
      if (paletteLine == NULL) {
        if (color.a() == TColor::fullAlphaTransparency) value = 0;
        else value = (color.r() & 0xF0) >> 4;
      }
      // palette
      else {
//        if (color.a() == TColor::fullAlphaTransparency) value = 0;
//        else
        value = paletteLine->matchColor(color, true);
      }
      
      if (value == -1) return -1;
      
      data_.data(i, j) = value;
    }
  }
  
  return 0;
}


}
