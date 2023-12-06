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
    cout << "PC-Engine to RGB color convertor" << endl;
    cout << "Usage: " << argv[0] << " <color>" << endl;
    
    return 0;
  }
  
  int rawColor = TStringConversion::stringToInt(string(argv[1]));
  
  PceColor color;
  color.fromNative(rawColor);
  TColor realColor = color.realColor();
  
  cout << as2bHex(realColor.r())
    << as2bHex(realColor.g())
    << as2bHex(realColor.b())
    << endl;
  
  return 0;
}
