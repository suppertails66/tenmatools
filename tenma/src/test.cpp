#include "tenma/TenmaMsgDism.h"
#include "tenma/TenmaMsgDismException.h"
#include "pce/PcePalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TSoundFile.h"
#include <string>
#include <iostream>

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
  TBufStream ifs;
  {
    TBufStream mapIfs;
    mapIfs.open("base/map4_11A7.bin");
    ifs.writeFrom(mapIfs, 0x2000);
  }
  
  ifs.seek(0);
  while (!ifs.eof()) {
    int basePos = ifs.tell();
    
    TenmaMsgDism dism;
    try {
      dism.dism(ifs,
                TenmaMsgDism::dismMode_full,
                TenmaMsgDism::msgTypeMode_check);
      std::cout << "valid: " << std::hex << basePos << std::endl;
    }
    catch (TenmaMsgDismException& e) {
//      std::cout << std::hex << basePos << ": " << e.problem() << std::endl;
      ifs.seek(basePos + 1);
    }
  }
  
  return 0;
}
