#ifndef PCETILEID_H
#define PCETILEID_H


#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "pce/PcePalette.h"

namespace Pce {


class PceTileId {
public:
  const static int size = 2;
  
  PceTileId();
  
  void read(const char* data);
  void write(char* data) const;
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
/*  void toColorGraphic(BlackT::TGraphic& dst,
                      const MdVram& vram,
                      const MdPalette& pal,
                      int x = 0, int y = 0) const;
  
  void toGrayscaleGraphic(BlackT::TGraphic& dst,
                      const MdVram& vram,
                      int x = 0, int y = 0) const; */
  
  int palette;
  int pattern;
  
protected:
  
};


}


#endif
