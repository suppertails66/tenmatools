#include "pce/PcePalette.h"
#include "pce/PcePattern.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include "util/TCsv.h"
#include "util/TSjis.h"
#include "util/TFileManip.h"
#include "util/TStringSearch.h"
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;
using namespace BlackT;
using namespace Pce;

const static int signPixelW = 16;
const static int signPixelH = 16;
const static int signPatternW = signPixelW / PcePattern::w;
const static int signPatternH = signPixelH / PcePattern::h;

struct SignPatternGroup {
  PcePattern ul;
  PcePattern ur;
  PcePattern ll;
  PcePattern lr;
};

void grpToSignPatternGroup(TGraphic& src, SignPatternGroup& dst) {
  dst.ul.fromGraphic(src, 0, 0, NULL, false);
  dst.ur.fromGraphic(src, 8, 0, NULL, false);
  dst.ll.fromGraphic(src, 0, 8, NULL, false);
  dst.lr.fromGraphic(src, 8, 8, NULL, false);
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "PC-Engine image decolorizer" << std::endl;
    std::cout << "Usage: " << argv[0] << " <infile> <paletteline> <outfile>"
      << std::endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string paletteLine(argv[2]);
  std::string outFile(argv[3]);

  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(inFile, grp);
  
  TGraphic output(grp.w(), grp.h());
  
  PcePaletteLine pal;
  {
    TBufStream ifs;
    ifs.open(paletteLine.c_str());
    pal.read(ifs);
  }
  
  int patternW = grp.w() / PcePattern::w;
  int patternH = grp.h() / PcePattern::h;
  
  for (int j = 0; j < patternH; j++) {
    for (int i = 0; i < patternW; i++) {
      int x = i * PcePattern::w;
      int y = j * PcePattern::h;
      
      PcePattern pattern;
      int result = pattern.fromGraphic(grp, x, y, &pal, false);
      if (result < 0) {
        std::cerr << "Conversion error at pattern ("
          << x
          << ", "
          << y
          << ")"
          << std::endl;
        return 1;
      }
      
      pattern.toGraphic(output, x, y, NULL, false);
    }
  }
  
  TPngConversion::graphicToRGBAPng(outFile, output);
  
  return 0;
}
