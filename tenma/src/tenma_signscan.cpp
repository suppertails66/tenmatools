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

// search up to this many patterns ahead from the initially matched UL
// pattern for the other three pieces before giving up
const static int nonstdSignScanMaxSize = 0x80;

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
  if (argc < 3) {
    std::cout << "Tengai Makyou Ziria sign graphic scanner" << std::endl;
    std::cout << "Usage: " << argv[0] << " <infile> <signgrp>" << std::endl;
    std::cout << "Results go to standard output." << std::endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string signGrpFile(argv[2]);

  TGraphic sheetGrp;
  TPngConversion::RGBAPngToGraphic(signGrpFile, sheetGrp);
  
  int numRows = sheetGrp.h() / signPixelH;
  int numCols = sheetGrp.w() / signPixelW;
  int numSigns = numRows * numCols;
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  std::vector<SignPatternGroup> signs;
  for (int j = 0; j < numRows; j++) {
    for (int i = 0; i < numCols; i++) {
      TGraphic signGrp(signPixelW, signPixelH);
      signGrp.copy(sheetGrp,
        TRect(0, 0, 0, 0),
        TRect(i * signPixelW, j * signPixelH, signPixelW, signPixelH));
      SignPatternGroup signPatterns;
      grpToSignPatternGroup(signGrp, signPatterns);
      signs.push_back(signPatterns);
    }
  }
  
  while (!ifs.eof()) {
    int pos = ifs.tell();
    PcePattern pattern;
    pattern.read(ifs);
    int finalPos = ifs.tell();
    
    for (int i = 0; i < signs.size(); i++) {
      SignPatternGroup& item = signs[i];
      
      if (item.ul == pattern) {
        // STANDARD PATTERN:
        // most signs are stored in map graphics to that they appear
        // visually complete when viewed at 16 tiles per row
        
        if (ifs.remaining() < 0x220) continue;
        
        PcePattern pattern_next;
        pattern_next.read(ifs);
        
        ifs.seek(pos + (PcePattern::size * 16));
        PcePattern pattern_down;
        pattern_down.read(ifs);
        
        PcePattern pattern_nextDown;
        pattern_nextDown.read(ifs);
        
        if ((item.ur != pattern_next)
            || (item.ll != pattern_down)
            || (item.lr != pattern_nextDown)) {
          
        }
        else {
          std::cout << "patchSignStd(ifs, "
            << i
            << ", " << TStringConversion::intToString(pos,
              TStringConversion::baseHex)
            << ");" << std::endl;
          continue;
        }
        
        // check for nonstandard patterns.
        // the rest of the sign could be anywhere, so we just have to scan
        // forward and hope for the best.
        
        // the UL tile usually occurs first, but there are cases where
        // it does not, such as suwako village
        int startPos = finalPos - (nonstdSignScanMaxSize * PcePattern::size);
        if (startPos < 0) startPos = 0;
//        ifs.seek(finalPos);
        ifs.seek(startPos);
        int endPos = finalPos + (nonstdSignScanMaxSize * PcePattern::size);
        
        bool urRemaining = true;
        bool llRemaining = true;
        bool lrRemaining = true;
        int urPos = 0;
        int llPos = 0;
        int lrPos = 0;
        while (!ifs.eof() && (ifs.tell() < endPos)) {
          if (!urRemaining && !llRemaining && !lrRemaining) {
            std::cout << "patchSignPiece(ifs, "
              << i
              << ", 0"
              << ", " << TStringConversion::intToString(pos,
                TStringConversion::baseHex)
              << ");" << std::endl;
            
            std::cout << "patchSignPiece(ifs, "
              << i
              << ", 1"
              << ", " << TStringConversion::intToString(urPos,
                TStringConversion::baseHex)
              << ");" << std::endl;
            
            std::cout << "patchSignPiece(ifs, "
              << i
              << ", 2"
              << ", " << TStringConversion::intToString(llPos,
                TStringConversion::baseHex)
              << ");" << std::endl;
            
            std::cout << "patchSignPiece(ifs, "
              << i
              << ", 3"
              << ", " << TStringConversion::intToString(lrPos,
                TStringConversion::baseHex)
              << ");" << std::endl;
            
            break;
          }
          
          int searchPos = ifs.tell();
          PcePattern nextPattern;
          nextPattern.read(ifs);
          
          if (urRemaining && (nextPattern == item.ur)) {
            urRemaining = false;
            urPos = searchPos;
          }
          else if (llRemaining && (nextPattern == item.ll)) {
            llRemaining = false;
            llPos = searchPos;
          }
          else if (lrRemaining && (nextPattern == item.lr)) {
            lrRemaining = false;
            lrPos = searchPos;
          }
        }
      }
    }
    
    ifs.seek(finalPos);
  }
  
  return 0;
}
