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

int patternsPerRow = 16;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "PC Engine-format raw sprite graphics dumper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile> [options]" << endl;
    
    cout << "Options: " << endl;
    cout << "  -s   " << "Set starting offset" << endl;
    cout << "  -r   " << "Set patterns per row" << endl;
    cout << "  -n   " << "Set number of patterns" << endl;
    cout << "  -p   " << "Set palette line" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outfile = argv[2];
  
  int startOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-s", &startOffset);
  
  TOpt::readNumericOpt(argc, argv, "-r", &patternsPerRow);
  
  PcePaletteLine palLine;
  bool hasPalLine = false;
  char* palOpt = TOpt::getOpt(argc, argv, "-p");
  if (palOpt != NULL) {
    TBufStream ifs;
    ifs.open(palOpt);
    palLine.read(ifs);
    
    hasPalLine = true;
  }
  
//  TIfstream ifs(infile, ios_base::binary);
  TBufStream ifs;
  ifs.open(infile);
  ifs.seek(startOffset);
  
  int numPatterns = (ifs.size() - startOffset)
    / PceSpritePattern::size;
  TOpt::readNumericOpt(argc, argv, "-n", &numPatterns);
  
  int outputW = patternsPerRow * PceSpritePattern::w;
  int outputH = (numPatterns / patternsPerRow) * PceSpritePattern::h;
  // deal with edge case
  if ((numPatterns % patternsPerRow) != 0) outputH += PceSpritePattern::h;
  
  TGraphic dst;
  dst.resize(outputW, outputH);
  dst.clearTransparent();
  
//  int pos = startOffset;
  int x = 0;
  int y = 0;
  for (int i = 0; i < numPatterns; i++) {
    PceSpritePattern pattern;
    pattern.read(ifs);
    
    if (hasPalLine) {
//      pattern.toColorGraphic(dst, palLine, x, y);
      pattern.toGraphic(dst, x, y, &palLine, true);
    }
    else {
//      pattern.toGrayscaleGraphic(dst, x, y);
      pattern.toGraphic(dst, x, y, NULL, true);
    }
    
    x += PceSpritePattern::w;
    if (((x / PceSpritePattern::w) % patternsPerRow) == 0) {
      x = 0;
      y += PceSpritePattern::h;
    }
  }
  
  TPngConversion::graphicToRGBAPng(string(outfile), dst);
  
  return 0;
}
