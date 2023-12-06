#include "smpce/SmPceGraphic.h"
#include "smpce/SmPceScriptOps.h"
#include "smpce/SmPceNameHasher.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


SmPceGraphic::SmPceGraphic() {
  // configure palette for 5-bit XGGGGGRRRRRBBBBB
/*  palette_.setProperties(5,
                        5, 10, 0,
                        0x03E0,
                        0x7C00,
                        0x001F); */
}

void SmPceGraphic::read(BlackT::TStream& ifs) {
  unk1 = ifs.get();
  unk2 = ifs.get();
  
  int patW = ifs.readu8();
  int patH = ifs.readu8();
  
  // sanity
  if (
//      (unk1 != 0)
//      || (unk2 != 0)
//      ||
      (patW > 0x40)
      || (patH > 0x40)) {
    return;
  }
  
  int expectedFileSize = 4 + 0x200 + (patW * patH) + (patW * patH * 32);
  if (ifs.remaining() != (expectedFileSize - 4)) {
    patW = 0;
    patH = 0; 
    return;
  }
      
  patterns_.resize(patW, patH);
  colorMap_.resize(patW, patH);
  
  // palette
//  palette_.read(ifs);
//  palette_.readRevEnd(ifs);
  
  // fuck this fucking 5-bit shit.
  // it fucks up all the color conversions.
  // just use the actual machine's colors.
  TBufStream convPal;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      int next = ifs.readu16be();
      int r = (next & 0x03E0) >> (5 + 2);
      int g = (next & 0x7C00) >> (10 + 2);
      int b = (next & 0x001F) >> (0 + 2);
      
      convPal.writeu16le((r << 3) | (g << 6) | b);
    }
  }
  convPal.seek(0);
  palette_.read(convPal);
  
  // color map
  for (int j = 0; j < patH; j++) {
    for (int i = 0; i < patW; i++) {
      colorMap_.data(i, j) = (ifs.get() & 0x0F);
    }
  }
  
  // pattern data
  for (int j = 0; j < patH; j++) {
    for (int i = 0; i < patW; i++) {
      patterns_.data(i, j).read(ifs);
    }
  }
}

void SmPceGraphic::write(BlackT::TStream& ofs) {
  int patW = patterns_.w();
  int patH = patterns_.h();
  int numPatterns = patW * patH;
  
  // header
  ofs.writeu8(unk1);
  ofs.writeu8(unk2);
  ofs.writeu8(patW);
  ofs.writeu8(patH);
  
  // palette
  PcePalette convertedPalette = palette_;
  convertedPalette.setProperties(5,
                        5, 10, 0,
                        0x03E0,
                        0x7C00,
                        0x001F);
  convertedPalette.writeRevEnd(ofs);
  
  // color map
  for (int i = 0; i < numPatterns; i++) {
    int x = (i % patW);
    int y = (i / patW);
    
    ofs.writeu8(colorMap_.data(x, y));
  }
  
  // patterns
  for (int i = 0; i < numPatterns; i++) {
    int x = (i % patW);
    int y = (i / patW);
    
    patterns_.data(x, y).write(ofs);
  }
}

void SmPceGraphic::save(const char* prefixstr) {
  int patW = patterns_.w();
  int patH = patterns_.h();
  
  // sanity
  if (
//      (unk1 != 0)
//      || (unk2 != 0)
//      || 
      (patW > 0x40)
      || (patH > 0x40)) {
    return;
  }
  
  // HACK: cheat!
//  if (palette_.paletteLines[0].colors[0].realColor().a()
//        == TColor::fullAlphaTransparency) return;
  if ((patW == 0) || (patH == 0)) return;
  
  std::string prefix(prefixstr);
  
  palette_.generatePreviewFile((prefix + "_pal.png").c_str());
  
  int realW = patW * PcePattern::w;
  int realH = patH * PcePattern::h;
  TGraphic g(realW, realH);
  g.clearTransparent();
  for (int j = 0; j < patH; j++) {
    for (int i = 0; i < patW; i++) {
      int x = i * PcePattern::w;
      int y = j * PcePattern::h;
      int palIndex = colorMap_.data(i, j);
      patterns_.data(i, j).toGraphic(
        g, x, y, &(palette_.paletteLines[palIndex]), false);
    }
  }
  TPngConversion::graphicToRGBAPng((prefix + "_grp.png"), g);
  
  TGraphic colorMapGrp(realW, realH);
  for (int j = 0; j < patH; j++) {
    for (int i = 0; i < patW; i++) {
      int x = i * PcePattern::w;
      int y = j * PcePattern::h;
      int palIndex = colorMap_.data(i, j);
      int value = palIndex << 4;
      
      TColor color(value, value, value, TColor::fullAlphaOpacity);
      colorMapGrp.fillRect(x, y, PcePattern::w, PcePattern::h, color,
                           TGraphic::noTransUpdate);
    }
  }
  TPngConversion::graphicToRGBAPng((prefix + "_map.png"), colorMapGrp);
}

//const PcePalette& SmPceGraphic::getPalette() const {
//  return palette_;
//}


}
