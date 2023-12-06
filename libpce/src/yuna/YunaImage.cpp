#include "yuna/YunaImage.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "exception/TGenericException.h"
#include "exception/TException.h"
#include <string>
#include <iostream>

using namespace BlackT;

namespace Pce {


YunaImage::YunaImage()
  : unknown1(0),
    unknown2(0) { }

void YunaImage::read(BlackT::TStream& ifs) {
  int size = ifs.readu16le();
  unknown1 = ifs.readu16le();
  unknown2 = ifs.readu16le();
  int tileW = ifs.readu16le();
  int tileH = ifs.readu16le();
  
  int tileDataStart = ifs.tell();
  ifs.seekoff(tileW * tileH * PcePattern::size);
  int numPaletteLines = ifs.readu16le();
  ifs.seekoff(numPaletteLines * PcePaletteLine::size);
  
  TBufStream paletteMapIfs;
  paletteMapIfs.writeFrom(ifs, tileW * tileH);
  paletteMapIfs.seek(0);
  
  tiles.resize(tileW, tileH);
  paletteMap.resize(tileW, tileH);
  
  // read tile data
  ifs.seek(tileDataStart);
  {
    int centerX = tileW / 2;
    int centerY = tileH / 2;
    
    int remainingW = tileW - 1;
    int remainingH = tileH - 1;
    
    // copy center tile
    tiles.data(centerX, centerY).read(ifs);
    paletteMap.data(centerX, centerY) = paletteMapIfs.readu8();
    
    int xCopySize = 3;
    int yCopySize = 1;
    int xCopyOffset = 1;
    int yCopyOffset = 1;
    while ((remainingH > 0) || (remainingW > 0)) {
      // top
      if (remainingH > 0) {
        int targetX = centerX - xCopyOffset;
        int targetY = centerY - yCopyOffset;
        
        if ((targetX >= 0) && (targetY >= 0)) {
          copyHorizontalRun(ifs, paletteMapIfs, targetX, targetY, xCopySize);
        }
        
        --remainingH;
      }
      
      // bottom
      if (remainingH > 0) {
        int targetX = centerX - xCopyOffset;
        int targetY = centerY + yCopyOffset;
        
        if ((targetX >= 0) && (targetY < tiles.h())) {
          copyHorizontalRun(ifs, paletteMapIfs, targetX, targetY, xCopySize);
        }
        
        --remainingH;
      }
      
      // left
      if (remainingW > 0) {
        int targetX = centerX - xCopyOffset;
        int targetY = centerY - yCopyOffset + 1;
        
        if ((targetX >= 0) && (targetY >= 0)) {
          copyVerticalRun(ifs, paletteMapIfs, targetX, targetY, yCopySize);
        }
        
        --remainingW;
      }
      
      // right
      if (remainingW > 0) {
        int targetX = centerX + xCopyOffset;
        int targetY = centerY - yCopyOffset + 1;
        
        if ((targetX < tiles.w()) && (targetY >= 0)) {
          copyVerticalRun(ifs, paletteMapIfs, targetX, targetY, yCopySize);
        }
        
        --remainingW;
      }
      
      // update fields
      xCopySize += 2;
      yCopySize += 2;
      xCopyOffset += 1;
      yCopyOffset += 1;
    }
  }
  
  ifs.seekoff(2);
  paletteLines.resize(numPaletteLines);
  for (int i = 0; i < numPaletteLines; i++) {
    paletteLines[i].read(ifs);
  }
//  std::cerr << numPaletteLines << std::endl;
  
/*  paletteMap.resize(tileW, tileH);
  for (int j = 0; j < tileH; j++) {
    for (int i = 0; i < tileW; i++) {
      paletteMap.data(i, j) = ifs.readu8();
    }
  } */
}

void YunaImage::write(BlackT::TStream& ofs) const {
  int startPos = ofs.tell();
  
  int tileW = tiles.w();
  int tileH = tiles.h();
  
  // size placeholder
  ofs.seekoff(2);
  ofs.writeu16le(unknown1);
  ofs.writeu16le(unknown2);
  ofs.writeu16le(tileW);
  ofs.writeu16le(tileH);
  
  // tile data
  TBufStream paletteMapOfs;
  {
    int centerX = tileW / 2;
    int centerY = tileH / 2;
    
    int remainingW = tileW - 1;
    int remainingH = tileH - 1;
    
    // copy center tile
    tiles.data(centerX, centerY).write(ofs);
    paletteMapOfs.writeu8(paletteMap.data(centerX, centerY));
    
    int xCopySize = 3;
    int yCopySize = 1;
    int xCopyOffset = 1;
    int yCopyOffset = 1;
    while ((remainingH > 0) || (remainingW > 0)) {
      // top
      if (remainingH > 0) {
        int targetX = centerX - xCopyOffset;
        int targetY = centerY - yCopyOffset;
        
        if ((targetX >= 0) && (targetY >= 0)) {
          writeHorizontalRun(ofs, paletteMapOfs, targetX, targetY, xCopySize);
        }
        
        --remainingH;
      }
      
      // bottom
      if (remainingH > 0) {
        int targetX = centerX - xCopyOffset;
        int targetY = centerY + yCopyOffset;
        
        if ((targetX >= 0) && (targetY < tiles.h())) {
          writeHorizontalRun(ofs, paletteMapOfs, targetX, targetY, xCopySize);
        }
        
        --remainingH;
      }
      
      // left
      if (remainingW > 0) {
        int targetX = centerX - xCopyOffset;
        int targetY = centerY - yCopyOffset + 1;
        
        if ((targetX >= 0) && (targetY >= 0)) {
          writeVerticalRun(ofs, paletteMapOfs, targetX, targetY, yCopySize);
        }
        
        --remainingW;
      }
      
      // right
      if (remainingW > 0) {
        int targetX = centerX + xCopyOffset;
        int targetY = centerY - yCopyOffset + 1;
        
        if ((targetX < tiles.w()) && (targetY >= 0)) {
          writeVerticalRun(ofs, paletteMapOfs, targetX, targetY, yCopySize);
        }
        
        --remainingW;
      }
      
      // update fields
      xCopySize += 2;
      yCopySize += 2;
      xCopyOffset += 1;
      yCopyOffset += 1;
    }
  }
  
  // palette lines
  ofs.writeu16le(paletteLines.size());
  for (int i = 0; i < paletteLines.size(); i++) {
    paletteLines[i].write(ofs);
  }
  
  // palette map
  paletteMapOfs.seek(0);
  while (!paletteMapOfs.eof()) ofs.put(paletteMapOfs.get());
  
  // finish with final size
  int endPos = ofs.tell();
  int size = endPos - startPos;
  ofs.seek(startPos);
  ofs.writeu16le(size);
  ofs.seek(endPos); 
}

void YunaImage::exportGrayscale(std::string filename) const {
  TGraphic grp;
  grp.resize(tiles.w() * PcePattern::w,
             tiles.h() * PcePattern::h);
  
  for (int j = 0; j < tiles.h(); j++) {
    for (int i = 0; i < tiles.w(); i++) {
      int x = (i * PcePattern::w);
      int y = (j * PcePattern::h);
      
      tiles.data(i, j).toGraphic(grp, x, y, NULL, false);
    }
  }
  
  TPngConversion::graphicToRGBAPng(filename, grp);
}

void YunaImage::exportColor(std::string filename) const {
  TGraphic grp;
  grp.resize(tiles.w() * PcePattern::w,
             tiles.h() * PcePattern::h);
  
  for (int j = 0; j < tiles.h(); j++) {
    for (int i = 0; i < tiles.w(); i++) {
      int x = (i * PcePattern::w);
      int y = (j * PcePattern::h);
      
      int palIndex = paletteMap.data(i, j) >> 4;
      const PcePaletteLine& paletteLine = paletteLines[palIndex];
      
      tiles.data(i, j).toGraphic(grp, x, y, &paletteLine, false);
    }
  }
  
  TPngConversion::graphicToRGBAPng(filename, grp);
}

void YunaImage::import(const BlackT::TGraphic& grp,
                       bool withColor) {
  // assume image dimensions already match
  
  for (int j = 0; j < tiles.h(); j++) {
    for (int i = 0; i < tiles.w(); i++) {
      int x = (i * PcePattern::w);
      int y = (j * PcePattern::h);
      
//      int palIndex = paletteMap.data(i, j) >> 4;
//      const PcePaletteLine& paletteLine = paletteLines[palIndex];
//      tiles.data(i, j).toGraphic(grp, x, y, &paletteLine, false);
      
      if (withColor) {
        bool success = false;
        for (int k = 0; k < paletteLines.size(); k++) {
          int result = tiles.data(i, j).fromGraphic(
                          grp, x, y, &paletteLines[k], true);
          if (result == 0) {
            paletteMap.data(i, j) = (k << 4);
            success = true;
            break;
          }
        }
        
        if (!success) {
          throw TGenericException(T_SRCANDLINE,
                                  "YunaImage::importColor()",
                                  std::string("Failed converting tile at (")
                                  + TStringConversion::intToString(i)
                                  + ","
                                  + TStringConversion::intToString(j)
                                  + ")");
        }
      }
      else {
        tiles.data(i, j).fromGraphic(grp, x, y, NULL, true);
      }
    }
  }
}

void YunaImage::copyHorizontalRun(
    BlackT::TStream& ifs, BlackT::TStream& paletteMapIfs,
    int targetX, int targetY, int xCopySize) {
  for (int i = 0; i < xCopySize; i++) {
    tiles.data(targetX + i, targetY).read(ifs);
    paletteMap.data(targetX + i, targetY) = paletteMapIfs.readu8();
  }
}

void YunaImage::copyVerticalRun(
    BlackT::TStream& ifs, BlackT::TStream& paletteMapIfs,
    int targetX, int targetY, int yCopySize) {   
  for (int i = 0; i < yCopySize; i++) {
    tiles.data(targetX, targetY + i).read(ifs);
    paletteMap.data(targetX, targetY + i) = paletteMapIfs.readu8();
  }
}

void YunaImage::writeHorizontalRun(
    BlackT::TStream& ofs, BlackT::TStream& paletteMapOfs,
    int targetX, int targetY, int xCopySize) const {
  for (int i = 0; i < xCopySize; i++) {
    tiles.data(targetX + i, targetY).write(ofs);
    paletteMapOfs.writeu8(paletteMap.data(targetX + i, targetY));
  }
}

void YunaImage::writeVerticalRun(
    BlackT::TStream& ofs, BlackT::TStream& paletteMapOfs,
    int targetX, int targetY, int yCopySize) const {
  for (int i = 0; i < yCopySize; i++) {
    tiles.data(targetX, targetY + i).write(ofs);
    paletteMapOfs.writeu8(paletteMap.data(targetX, targetY + i));
  }
}

}
