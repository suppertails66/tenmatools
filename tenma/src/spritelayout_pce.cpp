#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "util/TParse.h"
#include "pce/PceSpritePattern.h"
#include "pce/PcePalette.h"
#include "pce/PcePaletteLine.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace BlackT;
using namespace Pce;

//const static int patternsPerRow = 16;

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "PC Engine-format sprite layout patch utility" << endl;
    cout << "Usage: " << argv[0] << " <grp> <tileset> <layout>"
      << " <outfile> [options]" << endl;
    
    cout << "Options: " << endl;
    cout << "  -p   " << "Set palette line" << endl;
    
    return 0;
  }
  
  std::string grpFileName(argv[1]);
  std::string tileFileName(argv[2]);
  std::string layoutFileName(argv[3]);
  std::string outFileName(argv[4]);
  
  PcePaletteLine palLine;
  PcePaletteLine* palP = NULL;
//  bool hasPalLine = false;
  char* palOpt = TOpt::getOpt(argc, argv, "-p");
  if (palOpt != NULL) {
    TBufStream ifs;
    ifs.open(palOpt);
    palLine.read(ifs);
    
//    hasPalLine = true;
    palP = &palLine;
  }
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(grpFileName, grp);
  
  int tileW = grp.w() / PceSpritePattern::w;
  int tileH = grp.h() / PceSpritePattern::h;
  
//  std::ifstream layoutIfs;
//  layoutIfs.open(layoutFileName.c_str());
  TBufStream layoutIfs;
  layoutIfs.open(layoutFileName.c_str());
  
  TBufStream ofs;
  ofs.open(tileFileName.c_str());
  
  for (int j = 0; j < tileH; j++) {
    for (int i = 0; i < tileW; i++) {
      int id = TParse::matchInt(layoutIfs);
//      std::cerr << std::hex << id << std::endl;
      if (id == -1) continue;
      
      int srcX = i * PceSpritePattern::w;
      int srcY = j * PceSpritePattern::h;
      
//      TGraphic tileGrp;
//      tileGrp.copy(
//        grp, TRect(0, 0, PceSpritePattern::w, PceSpritePattern::h));
      
      PceSpritePattern sprite;
      sprite.fromGraphic(grp, srcX, srcY, palP);
      
      ofs.seek(id * PceSpritePattern::size);
      sprite.write(ofs);
    }
  }
  
/*  for (int i = 0; i < numPatterns; i++) {
    int x = (i % patternsPerRow) * PceSpritePattern::w;
    int y = (i / patternsPerRow) * PceSpritePattern::h;
    
    PceSpritePattern pattern;
    
    int result = 0;
    if (hasPalLine) {
      result = pattern.fromGraphic(grp, x, y, &palLine);
    }
    else {
      result = pattern.fromGraphic(grp, x, y, NULL);
    }
    
    if (result != 0) {
      cerr << "Error processing sprite " << i
        << " at (" << x << ", " << y << ")" << endl;
      return 1;
    }
    
    pattern.write(ofs);
  } */
  
  ofs.save(outFileName.c_str());
  
  return 0;
}
