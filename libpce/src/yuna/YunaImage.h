#ifndef YUNAIMAGE_H
#define YUNAIMAGE_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TTwoDArray.h"
#include "util/TByte.h"
#include "util/TGraphic.h"
#include "pce/PcePattern.h"
#include "pce/PcePaletteLine.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace Pce {


class YunaImage {
public:
  
  YunaImage();
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  void exportGrayscale(std::string filename) const;
  void exportColor(std::string filename) const;
  
  void import(const BlackT::TGraphic& grp,
              bool withColor = true);
  
protected:
  BlackT::TTwoDArray<PcePattern> tiles;
  BlackT::TArray<PcePaletteLine> paletteLines;
  BlackT::TTwoDArray<BlackT::TByte> paletteMap;
  
  int unknown1;
  int unknown2;
  
  void copyHorizontalRun(
    BlackT::TStream& ifs, BlackT::TStream& paletteMapIfs,
    int targetX, int targetY, int xCopySize);
  void copyVerticalRun(
    BlackT::TStream& ifs, BlackT::TStream& paletteMapIfs,
    int targetX, int targetY, int yCopySize);
  
  void writeHorizontalRun(
    BlackT::TStream& ofs, BlackT::TStream& paletteMapOfs,
    int targetX, int targetY, int xCopySize) const;
  void writeVerticalRun(
    BlackT::TStream& ofs, BlackT::TStream& paletteMapOfs,
    int targetX, int targetY, int yCopySize) const;
  
};


}


#endif
