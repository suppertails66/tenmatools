#include "pce/PcePattern.h"
#include <cstring>

using namespace BlackT;

namespace Pce {


PcePattern::PcePattern() {
  data_.resize(w, h);
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      data_.data(i, j) = 0;
    }
  }
}

bool PcePattern::operator==(const PcePattern& other) const {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      if (data_.data(i, j) != other.data_.data(i, j)) return false;
    }
  }
  
  return true;
}

bool PcePattern::operator!=(const PcePattern& other) const {
  return !(*this == other);
}

void PcePattern::read(TStream& ifs) {
  TByte buffer[size];
  ifs.read((char*)buffer, size);
  
  for (int j = 0; j < h; j++) {
    TByte mask = 0x80;
//    int shift = 7;
    int basePos = (j * 2);
    for (int i = 0; i < w; i++) {
      TByte result = 0;
    
    // how dumb am i
/*      int shift1 = shift - 0;
      int shift2 = shift - 1;
      int shift3 = shift - 2;
      int shift4 = shift - 3;
    
      // byte 1
      TByte byte1 = (buffer[basePos + 0] & mask);
      (shift1 >= 0) ? (byte1 >>= shift1) : (byte1 <<= -shift1);
      // byte 2
      TByte byte2 = (buffer[basePos + 1] & mask);
      (shift2 >= 0) ? (byte2 >>= shift2) : (byte2 <<= -shift2);
      // byte 3
      TByte byte3 = (buffer[basePos + 16] & mask);
      (shift3 >= 0) ? (byte3 >>= shift3) : (byte3 <<= -shift3);
      // byte 4
      TByte byte4 = (buffer[basePos + 17] & mask);
      (shift4 >= 0) ? (byte4 >>= shift4) : (byte4 <<= -shift4);
      
//      result |= ((buffer[basePos + 0] & mask) >> (shift - 0));
      // byte 2
//      result |= ((buffer[basePos + 1] & mask) >> (shift - 1));
      // byte 3
//      result |= ((buffer[basePos + 16] & mask) >> (shift - 2));
      // byte 4
//      result |= ((buffer[basePos + 17] & mask) >> (shift - 3));
      result = byte1 | byte2 | byte3 | byte4; */
      
      if ((buffer[basePos + 0] & mask) != 0)  result |= 0x01;
      if ((buffer[basePos + 1] & mask) != 0)  result |= 0x02;
      if ((buffer[basePos + 16] & mask) != 0) result |= 0x04;
      if ((buffer[basePos + 17] & mask) != 0) result |= 0x08;
      
      data_.data(i, j) = result;
      
      mask >>= 1;
//      --shift;
    }
  }
}

void PcePattern::write(TStream& ofs) const {
  TByte buffer[size];
  std::memset(buffer, 0, size);
  
  for (int j = 0; j < h; j++) {
    TByte mask = 0x80;
    int basePos = (j * 2);
    for (int i = 0; i < w; i++) {
      TByte src = data_.data(i, j);
      
      if ((src & 0x01) != 0) buffer[basePos + 0]  |= mask;
      if ((src & 0x02) != 0) buffer[basePos + 1]  |= mask;
      if ((src & 0x04) != 0) buffer[basePos + 16] |= mask;
      if ((src & 0x08) != 0) buffer[basePos + 17] |= mask;
      
      mask >>= 1;
    }
  }
  
  ofs.write((char*)buffer, size);
}

void PcePattern::toGraphic(BlackT::TGraphic& dst,
               int xOffset, int yOffset,
               const PcePaletteLine* paletteLine,
               bool transparency) const {
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
        color.setA(TColor::fullAlphaTransparency);
        dst.setPixel(x, y, color);
      }
      else {
        dst.setPixel(x, y, color);
      }
    }
  }
}

int PcePattern::fromGraphic(const BlackT::TGraphic& src,
                            int xOffset, int yOffset,
                            const PcePaletteLine* paletteLine,
                            bool transparency) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int x = xOffset + i;
      int y = yOffset + j;
      
      TColor color = src.getPixel(x, y);
      int value = -1;
      
//      std::cerr << std::dec << i << "," << j << std::endl;
//      std::cerr << std::hex << (unsigned int)color.r() << " " << (unsigned int)color.g() << " " << (unsigned int)color.b() << std::endl;
      
      // no palette
      if (paletteLine == NULL) {
        if (transparency && (color.a() == TColor::fullAlphaTransparency))
          value = 0;
        else
          value = (color.r() & 0xF0) >> 4;
      }
      // palette
      else {
//        if (color.a() == TColor::fullAlphaTransparency) value = 0;
//        else
        value = paletteLine->matchColor(color, transparency);
      }
      
      if (value == -1) return -1;
      
      data_.data(i, j) = value;
    }
  }
  
  return 0;
}

int PcePattern::getPixel(int x, int y) {
  return data_.data(x, y);
}


}
