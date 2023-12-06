#ifndef TRASTERFONT_H
#define TRASTERFONT_H


#include <vector>
#include <map>
#include <string>

namespace BlackT {


class TGraphic;

class TRasterFont {
public:
  TRasterFont();
  
  void loadAsciiMonospace(const std::string& filename, int w, int h);
  
  bool hasFontChar(int index) const;
  TGraphic& getFontChar(int index);
  const TGraphic& getFontChar(int index) const;
  
  void printAscii(TGraphic& dst, std::string src,
                  int x = 0, int y = 0) const;
  
protected:
  typedef std::map<int, TGraphic> IdGraphicMap;
  
  IdGraphicMap fontChars_;
};


}


#endif
