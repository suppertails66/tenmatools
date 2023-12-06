#include "tenma/TenmaTranslationSheet.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TFileManip.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TParse.h"
#include "util/TThingyTable.h"
#include <string>
#include <iostream>
#include <sstream>

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
    cout << "Word table printer" << endl;
    cout << "Usage: " << argv[0]
      << " <file> <offset> <count>" << endl;
    return 0;
  }
  
  std::string filename(argv[1]);
  int offset = TStringConversion::stringToInt(std::string(argv[2]));
  int count = TStringConversion::stringToInt(std::string(argv[3]));
  
  TBufStream ifs;
  ifs.open(filename.c_str());
  ifs.seek(offset);
  
  for (int i = 0; i < count; i++) {
    int word = ifs.readu16le();
    std::cout << ".dw $" << std::hex << word << std::endl;
  }
  
  return 0;
}
