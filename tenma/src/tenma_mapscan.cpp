#include "pce/PcePalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
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

const static int sectorSize = 0x800;

//TThingyTable tableStd;

struct FinalResult {
  int sectorPos;
  int sectorCount;
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Tengai Makyou Ziria map scanner" << std::endl;
    std::cout << "Usage: " << argv[0] << " <infile>" << std::endl;
    std::cout << "Results go to standard output." << std::endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  
//  tableStd.readUtf8("table/sjis_utf8_tenma_std.tbl");

  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  int numSectors = ifs.size() / sectorSize;
  std::vector<int> results;
  for (int i = 0; i < numSectors; i++) {
    int basePos = (i * sectorSize);
    ifs.seek(basePos + 0x16);
    int nameStrPtr = ifs.readu16le();
    
    if (nameStrPtr == 0x0000) {
      // TODO: is this allowed? it really shouldn't be
      continue;
    }
    
    if ((nameStrPtr < 0xC000) || (nameStrPtr >= 0xE000)) continue;
    
//    TBufStream checkIfs;
//    ifs.seek(basePos);
//    checkIfs.writeFrom(ifs, 0x1C * sectorSize);
    
    int nameOffset = nameStrPtr - 0xC000;
    ifs.seek(basePos + nameOffset);
    
    if (ifs.readu8() != 0x02) continue;
    
    TBufStream nameOfs;
    
    {
      bool good = false;
      
      while (!ifs.eof()) {
        // check for terminator
        if ((unsigned char)ifs.peek() == 0xFF) {
          if (nameOfs.size() > 0) {
            good = true;
          }
          break;
        }
        
        // max name len
        if ((nameOfs.size() / 2) > 8) break;
        
        // must be 2b sjis
        int next = ifs.readu16be();
        if (!TSjis::is2bSjis(next)) break;
        nameOfs.writeu16be(next);
      }
      
      if (!good) continue;
    }
    
    std::string outName;
    nameOfs.seek(0);
    while (!nameOfs.eof()) outName += nameOfs.get();
    
    std::cout << "Map "
      << TStringConversion::intToString(i, TStringConversion::baseHex)
      << ": "
      << outName
      << std::endl;
    
    results.push_back(i);
  }
  
  std::vector<FinalResult> finalResults;
  for (int i = 0; i < results.size() - 1; i++) {
    int current = results[i];
    int next = results[i + 1];
    
    if ((next - current) < 0x1A) {
      std::cout << "bad? " 
        << TStringConversion::intToString(current, TStringConversion::baseHex)
        << " / "
        << TStringConversion::intToString(next, TStringConversion::baseHex)
        << " = "
        << TStringConversion::intToString(next - current, TStringConversion::baseHex)
        << std::endl;
    }
    
    FinalResult result;
    result.sectorPos = current;
    if ((next - current) < 0x1A) {
      result.sectorCount = (next - current);
    }
    else {
      result.sectorCount = 0x1A;
    }
    
    finalResults.push_back(result);
  }
  
  {
    FinalResult last;
    last.sectorPos = results.back();
    last.sectorCount = 0x1A;
    finalResults.push_back(last);
  }
  
  for (const auto& result: finalResults) {
    std::cout << "VECTORNAME.push_back("
      << TStringConversion::intToString(result.sectorPos, TStringConversion::baseHex)
      << ");"
      << std::endl;
  }
  
  return 0;
}
