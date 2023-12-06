#include "pce/okiadpcm.h"
#include "pce/PcePalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TFileManip.h"
#include "util/TStringSearch.h"
#include "util/TParse.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Pce;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Disassembly quoter" << endl;
    cout << "Usage: " << argv[0] << " <infile>" << endl;
    return 0;
  }
  
  TBufStream ifs;
  ifs.open(argv[1]);
  
  while (!ifs.eof()) {
    std::string line;
    ifs.getLine(line);
    if (line.empty()) continue;
//    std::cerr << line << std::endl;
    
    TBufStream lineIfs;
    lineIfs.writeString(line);
    lineIfs.seek(0);
    
    std::string first = TParse::getSpacedSymbol(lineIfs);
    if ((first.size() > 0) && (first[0] == ';')) continue;
    
    TParse::skipSpace(lineIfs);
    char content[21];
    lineIfs.read(content, sizeof(content));
    int endpos = sizeof(content) - 1;
    while ((endpos > 0) && isspace(content[endpos])) --endpos;
    
    std::string quotestr(content, endpos + 1);
    quotestr += " ";
    std::cout << "    \"" << quotestr << "\"" << std::endl;
  }
  
  return 0;
}
