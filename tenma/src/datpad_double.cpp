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
  if (argc < 3) {
    cout << "Data padding utility" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outsize> <startpad> [outfile]"
      << " [padchar]"
      << endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  int outSize = TStringConversion::stringToInt(argv[2]);
  int startPad = TStringConversion::stringToInt(argv[3]);
  std::string outFile = inFile;
  if (argc >= 5)
    outFile = std::string(argv[4]);
  unsigned char padChar = 0xFF;
  if (argc >= 6)
    padChar = TStringConversion::stringToInt(argv[5]);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  TBufStream ofs;
  
  for (int i = 0; i < startPad; i++) ofs.put(padChar);
  ofs.writeFrom(ifs, ifs.size());
  while (ofs.size() < outSize) ofs.put(padChar);
  
  ofs.save(outFile.c_str());
  
  return 0;
}

