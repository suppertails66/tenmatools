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
#include "pce/PceSpriteId.h"
#include "pce/PcePalette.h"
#include "pce/PcePaletteLine.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace BlackT;
using namespace Pce;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Tengai Makyou Ziria sprite prep tool" << endl;
    cout << "Usage: " << argv[0] << " <infile> [outfile]" << endl;
    cout << "If omitted, outfile is same as infile." << endl;
    
/*    cout << "Options: " << endl;
    cout << "  -basepattern   " << "Sets base pattern number" << endl;
    cout << "  -xoffset       " << "X-offset applies to output" << endl;
    cout << "  -yoffset       " << "Y-offset applies to output" << endl;*/
    
    return 0;
  }
  
  string inFile = string(argv[1]);
  
  string outFile = inFile;
  if (argc >= 3)
    outFile = string(argv[2]);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  TBufStream ofs;
  
  int numSprites = ifs.size() / PceSpriteId::size;
  ofs.writeu8(numSprites);
  for (int i = 0; i < numSprites; i++) {
    int y = ifs.reads16le();
    int x = ifs.reads16le();
    int pattern = ifs.readu16le();
    int rawFlags = ifs.readu16le();
    
    if ((x >= 128) || (x < -128)) {
      std::cerr << "Error: out-of-range x-offset" << endl;
      return 1;
    }
    
    if ((y >= 128) || (y < -128)) {
      std::cerr << "Error: out-of-range y-offset" << endl;
      return 1;
    }
    
    int targetPattern = (pattern & 0x7FE) >> 1;
    
    // combine high byte of flags with pattern.
    // all fields are as normal except horizontal width,
    // which is stored in bit 6 instead of 0.
    // low 8 bits of low byte are pattern num.
    // low 3 bits of high byte are palette index (0-7).
    int rawFlagsHigh = (rawFlags & 0xFF00) >> 8;
    bool hasHflip = (rawFlagsHigh & 0x01) != 0;
    rawFlagsHigh &= 0xF8;
    if (hasHflip) rawFlagsHigh |= 0x40;
    // pattern offset is not stored preshifted
//    pattern >>= 1;
//    pattern &= 0xFF;
    // actually, forget this, we'll just write the pattern number
    // directly in the base pattern
    pattern = 0;
    pattern |= (rawFlagsHigh << 8);
    int palette = rawFlags & 0x07;
    pattern |= (palette << 8);
    
    ofs.writeu16le(pattern);
    ofs.writes8(x);
    ofs.writes8(y);
    // base pattern
    ofs.writeu16le((targetPattern << 1));
  }
  
  ofs.save(outFile.c_str());
  
  return 0;
}
