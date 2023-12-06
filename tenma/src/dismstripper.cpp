#include "smpce/SmPceMsgScriptDecmp.h"
#include "smpce/SmPceVarScriptDecmp.h"
#include "smpce/SmPceFileIndex.h"
#include "smpce/SmPceGraphic.h"
#include "pce/okiadpcm.h"
#include "pce/PcePalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TSoundFile.h"
#include <cctype>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
using namespace Pce;

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Disassembly stripper" << endl;
    cout << "Usage: " << argv[0] << " [infile] [stripwidth] [outfile]"
      << endl;
    return 0;
  }
  
  string infile(argv[1]);
  int stripWidth = TStringConversion::stringToInt(string(argv[2]));
  string outfile(argv[3]);

  TBufStream ifs;
  ifs.open(infile.c_str());
  
  std::ofstream ofs(outfile.c_str());
  
  while (!ifs.eof()) {
    std::string line;
    ifs.getLine(line);
    
//    if (line.size() < stripWidth) continue;
    
    bool fail = true;
    int start = 0;
    for ( ; start < line.size(); start++) {
      if (line[start] == ';') break;
      
      if (!isspace(line[start])) {
        fail = false;
        break;
      }
    }
    
    if ((line.size() - start) < stripWidth) fail = true;
    
    if (fail) {
      ofs << line << endl;
      continue;
    }
    
    ofs << line.substr(0, start);
    ofs << line.substr(start + stripWidth, string::npos) << endl;
  }
  
  return 0;
}
