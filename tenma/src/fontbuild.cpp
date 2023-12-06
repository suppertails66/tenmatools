#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TIniFile.h"
#include "util/TBufStream.h"
#include "util/TOfstream.h"
#include "util/TIfstream.h"
#include "util/TStringConversion.h"
#include "util/TBitmapFont.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace BlackT;

const static int charW = 8;
//const static int charH = 11;
// oh, apparently i actually made this font 10px high,
// so we can drop an extra row of pixels.
// wonderful.
static int charH = 10;

void charToData(const TGraphic& src,
                int xOffset, int yOffset,
                TStream& ofs) {
  for (int j = 0; j < charH; j++) {
    int output = 0;
    
    int mask = 0x80;
    for (int i = 0; i < charW; i++) {
      int x = xOffset + i;
      int y = yOffset + j;
      TColor color = src.getPixel(x, y);
      
      if ((color.a() == TColor::fullAlphaTransparency)
          || (color.r() < 0x80)) {
        
      }
      else {
        output |= mask;
      }
      
      mask >>= 1;
    }
    
    ofs.writeu8(output);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "1bpp font builder" << endl;
    cout << "Usage: " << argv[0] << " <font> <outfontfile> <outwidthfile>"
      << " [charH]"
      << endl;
    
    return 0;
  }
  
  string fontName(argv[1]);
  string outFontFileName(argv[2]);
  string outWidthFileName(argv[3]);
  
//  int charH = defaultCharH;
  if (argc >= 5) {
    charH = TStringConversion::stringToInt(string(argv[4]));
  }
  
  TBitmapFont font;
  font.load(fontName);
  
  TBufStream fontofs;
  TBufStream widthofs;
  
  for (int i = 0; i < font.numFontChars(); i++) {
    const TBitmapFontChar& fontChar = font.fontChar(i);
    int width = fontChar.advanceWidth;
    
    charToData(fontChar.grp, 0, 0, fontofs);
    widthofs.writeu8(width);
  }
  
  fontofs.save(outFontFileName.c_str());
  widthofs.save(outWidthFileName.c_str());
  
  return 0;
}
