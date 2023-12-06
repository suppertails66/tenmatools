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
#include "pce/PceSpritePattern.h"
#include "pce/PcePalette.h"
#include "pce/PcePaletteLine.h"
#include <iostream>
#include <string>

using namespace std;
using namespace BlackT;
using namespace Pce;

//const static int patternsPerRow = 16;

int spritePatternW = 2;
int spritePatternH = 4;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "PC Engine-format raw var-size sprite graphics undumper" << endl;
    cout << "Usage: " << argv[0] << " <graphic> <outfile> [options]" << endl;
    
    cout << "Options: " << endl;
    cout << "  -p   " << "Set palette line" << endl;
    cout << "  -r   " << "Set patterns per row" << endl;
    cout << "  -n   " << "Set number of input patterns (default: auto)"
      << endl;
    cout << "  -w   " << "Set width of each sprite in patterns"
      << endl;
    cout << "  -h   " << "Set height of each sprite in patterns"
      << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outfile = argv[2];
  
  PcePaletteLine palLine;
  bool hasPalLine = false;
  char* palOpt = TOpt::getOpt(argc, argv, "-p");
  if (palOpt != NULL) {
    TBufStream ifs;
    ifs.open(palOpt);
    palLine.read(ifs);
    
    hasPalLine = true;
  }
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(std::string(infile), grp);
  
  TOpt::readNumericOpt(argc, argv, "-w", &spritePatternW);
  TOpt::readNumericOpt(argc, argv, "-h", &spritePatternH);
  
  int fullSpriteW = spritePatternW * PceSpritePattern::w;
  int fullSpriteH = spritePatternH * PceSpritePattern::h;
  
  int patternsPerRow = grp.w() / fullSpriteW;
  
  // override patterns per row if provided
  TOpt::readNumericOpt(argc, argv, "-r", &patternsPerRow);
  
  int numRows = grp.h() / fullSpriteH;
  int numPatterns = (patternsPerRow * numRows);
  
  // override number of patterns if provided
  TOpt::readNumericOpt(argc, argv, "-n", &numPatterns);
  
  TBufStream ofs;
  
  for (int i = 0; i < numPatterns; i++) {
    int baseX = (i % patternsPerRow) * fullSpriteW;
    int baseY = (i / patternsPerRow) * fullSpriteH;
    
    for (int j = 0; j < fullSpriteH; j += PceSpritePattern::h) {
      for (int i = 0; i < fullSpriteW; i += PceSpritePattern::w) {
        int x = baseX + i;
        int y = baseY + j;
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
      }
    }
  }
  
  ofs.save(outfile);
  
  return 0;
}
