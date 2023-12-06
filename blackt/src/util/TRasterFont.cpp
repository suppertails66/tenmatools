#include "util/TRasterFont.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"

namespace BlackT {


TRasterFont::TRasterFont() { }
  
void TRasterFont
  ::loadAsciiMonospace(const std::string& filename, int w, int h) {
  TGraphic src;
  TPngConversion::RGBAPngToGraphic(filename, src);
  
  int charsPerRow = src.w() / w;
  int rows = src.h() / h;
  
  int index = 0;
  for (int j = 0; j < rows; j++) {
    for (int i = 0; i < charsPerRow; i++) {
      int x = (i * w);
      int y = (j * h);
      
//      std::cerr << x << " " << y << std::endl;
      
      TGraphic g(w, h);
//      g.clearTransparent();
      g.copy(src,
             TRect(0, 0, 0, 0),
             TRect(x, y, w, h),
             TGraphic::transUpdate);
      fontChars_[index + 0x20] = g;
      ++index;
    }
  }
}

bool TRasterFont::hasFontChar(int index) const {
  IdGraphicMap::const_iterator findIt = fontChars_.find(index);
  if (findIt == fontChars_.end()) return false;
  return true;
}

TGraphic& TRasterFont::getFontChar(int index) {
  return fontChars_.find(index)->second;
}

const TGraphic& TRasterFont::getFontChar(int index) const {
  return fontChars_.find(index)->second;
}

void TRasterFont::printAscii(TGraphic& dst, std::string src,
                             int x, int y) const {
  int initialX = x;
//  std::cerr << fontChars_.size() << std::endl;
  for (int i = 0; i < src.size(); i++) {
    int index = (unsigned char)(src[i]);
    
//    std::cerr << index << std::endl;
    
    if (index == '\n') {
      x = initialX;
      y += fontChars_.at(0x20).h();    // height of space character
    }
    else {
//      const TGraphic& grp = fontChars_[index];
      const TGraphic& grp = fontChars_.at(index);
      dst.blit(grp,
               TRect(x, y, 0, 0));
      x += grp.w();
    }
  }
}


}
