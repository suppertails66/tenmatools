#ifndef PCESPRITEID_H
#define PCESPRITEID_H


#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "pce/PcePalette.h"

namespace Pce {


class PceSpriteId {
public:
  const static int size = 8;
  
  PceSpriteId();
  
//  void read(const char* data);
//  void write(char* data) const;
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  /*Each SAT entry has the following format:

 Word 0 : ------aaaaaaaaaa
 Word 1 : ------bbbbbbbbbb
 Word 2 : -----ccccccccccd
 Word 3 : e-ffg--hi---jjjj

 a = Sprite Y position (0-1023)
 b = Sprite X position (0-1023)
 c = Pattern index (0-1023)
 d = CG mode bit (0= Read bitplanes 0/1, 1= Read bitplanes 2/3)
 e = Vertical flip flag 
 f = Sprite height (CGY) (0=16 pixels, 1=32, 2/3=64)
 g = Horizontal flip flag
 h = Sprite width (CGX) (0=16 pixels, 1=32)
 i = Sprite priority flag (1= high priority, 0= low priority)
 j = Sprite palette (0-15)*/
  int y;
  int x;
  int pattern;
  bool cgMode;
  bool vFlip;
  int height;
  bool hFlip;
  int width;
  bool priority;
  int palette;
  
protected:
  
};


}


#endif
