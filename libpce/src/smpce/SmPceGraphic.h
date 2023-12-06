#ifndef SMPCEGRAPHIC_H
#define SMPCEGRAPHIC_H


#include "util/TStream.h"
#include "util/TByte.h"
#include "util/TTwoDArray.h"
#include "pce/PcePattern.h"
#include "pce/PcePalette.h"
#include <string>
#include <vector>
#include <iostream>

namespace Pce {


class SmPceGraphic {
public:
  
  SmPceGraphic();
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs);
  
  void save(const char* prefixstr);
  
//  const PcePalette& getPalette() const;
  typedef BlackT::TTwoDArray<PcePattern> PatternArray;
  typedef BlackT::TTwoDArray<BlackT::TByte> ByteArray;
  
  BlackT::TByte unk1;
  BlackT::TByte unk2;
  PatternArray patterns_;
  ByteArray colorMap_;
  PcePalette palette_;
  
protected:
  
  
};


}


#endif
