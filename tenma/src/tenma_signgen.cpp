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
  if (argc < 5) {
    std::cout << "Tengai Makyou Ziria sign data generator" << std::endl;
    std::cout << "Usage: " << argv[0] << " <signgrp> <x> <y> <outprefix>" << std::endl;
    
    return 0;
  }
  
  std::string signGrpFile(argv[1]);
  int srcX = TStringConversion::stringToInt(std::string(argv[2]));
  int srcY = TStringConversion::stringToInt(std::string(argv[3]));
  std::string outPrefix(argv[4]);

  TGraphic sheetGrp;
  TPngConversion::RGBAPngToGraphic(signGrpFile, sheetGrp);
  
  TGraphic signGrp(signPixelW, signPixelH);
  signGrp.copy(sheetGrp,
    TRect(0, 0, 0, 0),
    TRect(srcX, srcY, signPixelW, signPixelH));
  SignPatternGroup signPatterns;
  grpToSignPatternGroup(signGrp, signPatterns);
  
  {
    TBufStream ofs;
    signPatterns.ul.write(ofs);
    ofs.save((outPrefix + "_0.bin").c_str());
  }
  
  {
    TBufStream ofs;
    signPatterns.ur.write(ofs);
    ofs.save((outPrefix + "_1.bin").c_str());
  }
  
  {
    TBufStream ofs;
    signPatterns.ll.write(ofs);
    ofs.save((outPrefix + "_2.bin").c_str());
  }
  
  {
    TBufStream ofs;
    signPatterns.lr.write(ofs);
    ofs.save((outPrefix + "_3.bin").c_str());
  }
  
  return 0;
}
