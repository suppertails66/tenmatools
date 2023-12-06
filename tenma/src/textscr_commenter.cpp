#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace BlackT;

typedef std::map<unsigned long int, int> AddrToOpMap;
AddrToOpMap textscrAddrToOpMap;

string as3bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 3) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

const int tableBase = 0x8701 - 0x8000;
const int numOps = 0x2E - 1;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Textscript disassembly comment label adder" << endl;
    cout << "Usage: " << argv[0] << " [dis_infile] [dis_outfile]" << endl;
    
    return 0;
  }
  
  std::ifstream fileifs(argv[1]);
  std::ofstream fileofs(argv[2]);
  
  {
    TBufStream ifs;
//    ifs.open("files_sat/1ST_MAHO.BIN");
    ifs.open("base/scriptmod_386.bin");
    ifs.seek(tableBase);
    for (int i = 0; i < numOps; i++) {
/*      ifs.seek(tableBase + i);
      int lo = ifs.readu8();
      ifs.seek(tableBase + numOps + i);
      int hi = ifs.readu8();
      
      int value = lo | (hi << 8);
      cerr << hex << value << endl; */
      
      unsigned int value = ifs.readu16le();
      cerr << hex << value << endl;
      
      textscrAddrToOpMap[value] = i + 1;
//      cout << TStringConversion::intToString(value, TStringConversion::baseHex)
//        << endl;
    }
  }
  
  while (fileifs.good()) {
    fileifs.get();
    if (!fileifs.good()) break;
    fileifs.unget();
    
    std::string line;
    std::getline(fileifs, line);
    
    std::istringstream ifs;
    ifs.str(line);
    
    std::string start;
    ifs >> start;
    
    // comments
    if ((start.size() == 0)
        || ((start.size() > 0) && (start[0] == ';'))) {
      fileofs << line << endl;
      continue;
    }
//    TBufStream ifs;
//    ifs.writeString(line);
    
    std::string numstr = string("0x") + start;
    unsigned long int addr = TStringConversion::stringToInt(numstr);
    
//    cout << hex << addr << endl;
//    char c; cin >> c;
    
    AddrToOpMap::iterator findIt = textscrAddrToOpMap.find(addr);
    if (findIt != textscrAddrToOpMap.end()) {
      int opNum = findIt->second;
      
      fileofs << endl << "; textscript op " << as2bHex(opNum) << endl << endl;
      fileofs << line << endl;
    }
    else {
      fileofs << line << endl;
    }
  }
  
  return 0;
}
