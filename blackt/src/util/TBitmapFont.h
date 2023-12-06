#ifndef TBITMAPFONT_H
#define TBITMAPFONT_H


#include "util/TStream.h"
#include "util/TBitmapFontChar.h"
#include "util/TTwoDArray.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>

namespace BlackT {


class TBitmapFont {
public:
  TBitmapFont();
  
  void load(std::string inprefix);
  
//  void render(TGraphic& dst, const std::string& msg,
//              const TThingyTable& table) const;
  void render(TGraphic& dst, TStream& msg,
              const TThingyTable& table) const;
  
  int numFontChars() const;
  TBitmapFontChar& fontChar(int index);
  const TBitmapFontChar& fontChar(int index) const;
  void exportKerningMatrix(TStream& ofs) const;
  
  int getKerning(int first, int second) const;
  void setKerning(int first, int second, int value);
  void offsetKerning(int first, int second, int offset);
protected:
  typedef std::vector<TBitmapFontChar> FontCharCollection;
  typedef TTwoDArray<char> KerningMatrix;
  
  FontCharCollection fontChars;
  KerningMatrix kerningMatrix;
  int charH;
  
  static bool pixelIsNotBackground(const TGraphic& grp, int x, int y);
  static int computeKerning(
    const TBitmapFontChar& first, const TBitmapFontChar& second);
  
};


}


#endif
