#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TOpt.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "pce/PcePattern.h"
#include "pce/PcePalette.h"
#include "pce/PceTilemap.h"
#include "pce/PceVram.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Pce;

int main(int argc, char* argv[]) {
  
  // Input:
  // * input tiles
  // * input tilemap
  // * output graphic filename
  //
  // options:
  // * palette (default: grayscale)
  // * VRAM load position (default: 0)
  // * output palette preview?
  
  int vramLoadPos = 0;
//  bool hasPalette = false;
//  PcePalette palette;
  PcePalette* palette = NULL;
  PcePalette paletteObj;
  int srcTilemapOffset = 0;
  
  if (argc < 6) {
    cout << "Utility to convert PC Engine tiles and maps to graphics" << endl;
    cout << "Usage: " << argv[0] << " <tiles> <tilemap> <tilew> <tileh>"
      " <outfile> [options]"
      << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -p   Input palette file (default: grayscale)" << endl;
    cout << "  -v   Tile VRAM load position (default: "
      << vramLoadPos << ")" << endl;
    cout << "  -o   Sets starting offset in tilemap file" << endl;
    cout << "  -t   Enables transparency on color 0" << endl;
    
    return 0;
  }
  
  char* tilesName = argv[1];
  char* tilemapName = argv[2];
  int tileW = TStringConversion::stringToInt(string(argv[3]));
  int tileH = TStringConversion::stringToInt(string(argv[4]));
  char* outfileName = argv[5];
  
  // Read arguments
  
  char* paletteFileName = TOpt::getOpt(argc, argv, "-p");
  if (paletteFileName != NULL) {
//    hasPalette = true;
    TBufStream ifs;
    ifs.open(paletteFileName);
    paletteObj.read(ifs);
    palette = &paletteObj;
  }
  
  TOpt::readNumericOpt(argc, argv, "-v", &vramLoadPos);
  
  TOpt::readNumericOpt(argc, argv, "-o", &srcTilemapOffset);
  
  bool transparency = TOpt::hasFlag(argc, argv, "-t");
  
  // Set up VRAM
  PceVram vram;
  {
    TBufStream ifs;
    ifs.open(tilesName);
    vram.readPatterns(ifs, vramLoadPos);
  }
  
  // Read tilemap
  PceTilemap tilemap;
  {
    TBufStream ifs;
    ifs.open(tilemapName);
    tilemap.read(ifs, tileW, tileH);
  }
  
  // Convert to graphic
  TGraphic g;
  tilemap.toGraphic(g, vram, palette, transparency);
  
  // Save result
  TPngConversion::graphicToRGBAPng(string(outfileName), g);
  
  return 0;
}
