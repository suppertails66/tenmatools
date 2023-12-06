#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "util/TBitmapFont.h"
#include "util/TThingyTable.h"
#include <iostream>
#include <string>

using namespace std;
using namespace BlackT;
//using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Font renderer" << endl;
    cout << "Usage: " << argv[0]
      << " <fontprefix> <stringfile> <table> <outfile>"
      << endl;
    
    return 0;
  }
  
  char* fontPrefix = argv[1];
  char* stringfile = argv[2];
  char* tablefile = argv[3];
  char* outfile = argv[4];
  
  TBitmapFont font;
  font.load(std::string(fontPrefix));
  
  TBufStream ifs;
  {
    TBufStream filterIfs;
    filterIfs.open(stringfile);
    while (!filterIfs.eof()) {
      if ((filterIfs.peek() == '\r')
          || (filterIfs.peek() == '\n')) {
        break;
      }
      
      ifs.put(filterIfs.get());
    }
  }
  ifs.seek(0);
  
  TThingyTable table;
  table.readSjis(tablefile);
  
  TGraphic grp;
  font.render(grp, ifs, table);
  
  TPngConversion::graphicToRGBAPng(std::string(outfile), grp);
  
  return 0;
}
