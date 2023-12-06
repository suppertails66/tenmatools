#include "util/TFreeSpace.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TStringSearch.h"
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace BlackT;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "fix for shitty outdated openoffice xlsx conversion shenanigans"
      << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>"
      << endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string outFile(argv[2]);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  TBufStream ofs;
  
  while (!ifs.eof()) {
    // check for "<t>"
    int pos = ifs.tell();
    if (TStringSearch::checkString(ifs, "3C 74 3E")) {
//      std::cerr << "here" << std::endl;
      // replace with corrected sequence
      ofs.writeString(std::string("<t xml:space=\"preserve\">"));
      ifs.seek(pos + 3);
    }
    else {
      ifs.seek(pos);
      ofs.put(ifs.get());
    }
  }
  
  ofs.save(outFile.c_str());
  
  return 0;
}

