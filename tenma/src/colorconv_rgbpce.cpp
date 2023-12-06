#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TOpt.h"
#include "pce/PcePattern.h"
#include "pce/PcePalette.h"
#include "pce/PcePaletteLine.h"
#include <iostream>
#include <string>

using namespace std;
using namespace BlackT;
using namespace Pce;

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "RGB to PC-Engine color convertor" << endl;
    cout << "Usage: " << argv[0] << " <RRGGBB>" << endl;
    
    return 0;
  }
  
  string rawColorStr = string(argv[1]);
  string rStr = string("0x") + rawColorStr.substr(0, 2);
  string gStr = string("0x") + rawColorStr.substr(2, 2);
  string bStr = string("0x") + rawColorStr.substr(4, 2);
  
  int r = TStringConversion::stringToInt(rStr);
  int g = TStringConversion::stringToInt(gStr);
  int b = TStringConversion::stringToInt(bStr);
  TColor realColor(r, g, b);
  
  PceColor color;
  color.setRealColor(realColor);
  
  cout << TStringConversion::intToString(color.getNative(),
            TStringConversion::baseHex)
       << endl;
  
  return 0;
}
