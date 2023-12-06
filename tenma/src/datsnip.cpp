#include "util/TFreeSpace.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace BlackT;

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Data snipping utility" << endl;
    cout << "Usage: " << argv[0] << " <infile> <srcoffset> <size> <outfile>"
//      << " [srcoffset] [srclen]"
      << endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  int srcOffset = TStringConversion::stringToInt(argv[2]);
  int srcSize = TStringConversion::stringToInt(argv[3]);
  std::string outFile(argv[4]);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  ifs.seek(srcOffset);
  
  TBufStream ofs;
  ofs.writeFrom(ifs, srcSize);
  
  ofs.save(outFile.c_str());
  
  return 0;
}

