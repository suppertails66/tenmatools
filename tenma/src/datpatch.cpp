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

int nextInt(istream& ifs) {
  string str;
  ifs >> str;
  return TStringConversion::stringToInt(str);
}

/*int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Data patching utility" << endl;
    cout << "Usage: " << argv[0] << " <srcrom> <dstrom> <srcdata> <offset>"
      << " [srcoffset] [srclen]"
      << endl;
    
    return 0;
  }
  
  int srcoffset = 0;
  int srclen = -1;
  
  if (argc >= 6) srcoffset = TStringConversion::stringToInt(argv[5]);
  
  if (argc >= 7) srclen = TStringConversion::stringToInt(argv[6]);
  
//  TBufStream ofs(0x800000);
//  {
//    TIfstream ifs(argv[1], ios_base::binary);
//    ofs.writeFrom(ifs, ifs.size());
//  }
//  int romSize = ofs.tell();
  TBufStream ofs;
  if (TFileManip::fileExists(std::string(argv[1])) {
    ofs.open(argv[1]);
  }
  
  int offset = TStringConversion::stringToInt(string(argv[4]));
    
  TIfstream ifs(argv[3], ios_base::binary);
  
  if (srclen == -1) srclen = ifs.size();
  
  ofs.seek(offset);
  ifs.seek(srcoffset);
  ofs.writeFrom(ifs, srclen);
  
  ofs.save(argv[2]);
  
  return 0;
} */

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Data patching utility" << endl;
    cout << "Usage: " << argv[0] << " <srcrom> <dstrom> <srcdata> <dstoffset>"
      << " [srcoffset] [srclen]"
      << endl;
    
    return 0;
  }
  
  int srcoffset = 0;
  int srclen = -1;
  
  if (argc >= 6) srcoffset = TStringConversion::stringToInt(argv[5]);
  
  if (argc >= 7) srclen = TStringConversion::stringToInt(argv[6]);
  
  TBufStream ofs;
  {
    TIfstream ifs(argv[1], ios_base::binary);
    ofs.writeFrom(ifs, ifs.size());
  }
  int romSize = ofs.tell();
  
  int offset = TStringConversion::stringToInt(string(argv[4]));
    
  TIfstream ifs(argv[3], ios_base::binary);
  
  ofs.seek(offset);
  ifs.seek(srcoffset);
  
//  if (srclen == -1) srclen = ifs.remaining();
  if (srclen == -1) srclen = ofs.remaining();
  
  ofs.writeFrom(ifs, srclen);
  
  ofs.seek(romSize);
  ofs.save(argv[2]);
  
  return 0;
}

