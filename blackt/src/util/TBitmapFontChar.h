#ifndef TBITMAPFONTCHAR_H
#define TBITMAPFONTCHAR_H


#include "util/TStream.h"
#include "util/TGraphic.h"
#include "util/TIniFile.h"
#include <string>

namespace BlackT {


class TBitmapFontChar {
public:
  friend class TBitmapFont;
  
  TBitmapFontChar();
  
  void readFromIni(TIniFile& ini, std::string& section);
  
  TGraphic grp;
  int glyphWidth;
  int advanceWidth;
  bool noKerningIfFirst;
  bool noKerningIfSecond;
protected:
//  bool has_firstKerningAgainstShortChars;
//  int firstKerningAgainstShortChars;
//  bool has_secondKerningAgainstShortChars;
//  int secondKerningAgainstShortChars;
  
};


}


#endif
