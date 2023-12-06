#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TOpt.h"
#include "util/TSjis.h"
#include "util/TThingyTable.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace BlackT;

struct StringCheckResult {
  int rawLength;
  int charCount;
  std::string str;
};

bool between(int value, int lower, int upper) {
  return (((value < lower) || (value > upper)) ? false : true);
}

// checks potential 2-byte sequences only
bool isSjis(int code) {
/*  int lowbyte = code & 0x00FF;
  if ((lowbyte < 0x40)
      || (lowbyte >= 0xFD)) return false;

  // oh boy
  if (between(code, 0x81AD, 0x81B7)
      || between(code, 0x81C0, 0x81C7)
      || between(code, 0x81CF, 0x81D9)
      || between(code, 0x81E9, 0x81EF)
      || between(code, 0x81F8, 0x81FB)
      || between(code, 0x8240, 0x824E)
      || between(code, 0x8259, 0x825F)
      || between(code, 0x827A, 0x8280)
      || between(code, 0x829B, 0x829E)
      || between(code, 0x82F2, 0x82FC)
      || between(code, 0x8392, 0x839E)
      || between(code, 0x83B7, 0x83BE)
      || between(code, 0x83B7, 0x83BE)
      || between(code, 0x83D7, 0x83FC)
      || between(code, 0x8461, 0x846F)
      || between(code, 0x8492, 0x849E)
      || between(code, 0x84BF, 0x86FC)
      || (code == 0x875E)
      || between(code, 0x8776, 0x877D)
      || between(code, 0x879D, 0x889E)
      || between(code, 0xEAA5, 0xECFC)
      || between(code, 0xEEED, 0xEEEE)
      || between(code, 0xEF40, 0xFFFF)
      ) return false;
  
  return true;*/
  return TSjis::isSjis(code);
}

int checkChar(TStream& ifs, TThingyTable& table) {
  unsigned char next = ifs.get();
  
  // null == terminator
  if (next == 0) return -1;
  
/*    if (halfwidth) {
    if (!between(next, 0xA0, 0xDF)) return -1;
    
    ++len;
  }
  else {
    // need 2 chars
    if (pos + len + 1 >= sz) return -1;
    
    if (!(between(next, 0x20, 0x40)
        || between(next, 0x81, 0x9F)
        || between(next, 0xE0, 0xF0))) return -1;
    
    if (next == 0x20) ++len;
    else len += 2;
  } */
  
  // two-byte sequence?
  if (between(next, 0x81, 0x9F)
      || between(next, 0xE0, 0xEF)) {
    if (ifs.remaining() < 1) return -1;
    unsigned char nextnext = ifs.get();
    int code = nextnext;
    code |= ((int)next << 8);
    
    if (isSjis(code)) return 2;
    else return -1;
  }
  
  // one-byte sequence?
  if ((next == 0x0A)    // cr
      || (next == 0x0D) // lf
      || between(next, 0x20, 0x7F)
      || between(next, 0xA1, 0xDF)) return 1;
  
  // invalid
  return -1;
}

StringCheckResult checkString(TStream& ifs, TThingyTable& table, char terminator) {
  int start = ifs.tell();
  
  StringCheckResult result;
  result.rawLength = -1;
  result.charCount = -1;
  
  int len = 0;
  int charCount = 0;
  
  // assume no intermixed full/halfwidth sequences
//  bool halfwidth = false;
//  if (between(buffer[pos], 0xA0, 0xDF)) {
//    halfwidth = true;
//  }
  
  bool terminated = false;
  while (!ifs.eof()) {
    if (ifs.peek() == terminator) {
      terminated = true;
      break;
    }
    
//    int nextCharSize = checkChar(ifs);
//    if (nextCharSize <= 0) break;
    int charStart = ifs.tell();
    TThingyTable::MatchResult matchResult = table.matchId(ifs);
    if (matchResult.id == -1) break;
    result.str += table.getEntry(matchResult.id);
    
    len += (ifs.tell() - charStart);
    
    ++charCount;
  }
  
  // rewind stream to initial position
  ifs.seek(start);
  
  // if the null string, or no terminator found
  if (!terminated || (len == 0) || ifs.eof()) return result;
  
//  std::cerr << std::hex << start << " " << std::hex << len + 1 << std::endl;
  
  result.rawLength = len + 1; // include terminator in length count
  result.charCount = charCount;
  return result;
}

int main(int argc, char* argv[]) {
  int minimumStringLen = 4;
  int initialOffset = 0;
//  int scanSize = -1;
  int endPos = -1;
  char terminator = 0xFF;

  if (argc < 3) {
    cout << "Tengai Makyou Ziria string finder" << endl;
    cout << "Usage: " << argv[0] << " <file> <table> [options]" << endl;
    cout << "Options: " << endl;
    cout
      << "  -l     Minimum characters required for a successful match" << endl
      << "         (default: " << minimumStringLen << ")" << endl
      << "  -s     Set starting offset of scan" << endl
      << "  -f     Set ending offset of scan (default: end of file)" << endl;
    return 0;
  }
  
  TBufStream ifs;
  ifs.open(argv[1]);
  
  TThingyTable table;
  table.readUtf8(argv[2]);
  
  TOpt::readNumericOpt(argc, argv, "-l", &minimumStringLen);
  TOpt::readNumericOpt(argc, argv, "-s", &initialOffset);
//  TOpt::readNumericOpt(argc, argv, "-s", &scanSize);
  TOpt::readNumericOpt(argc, argv, "-f", &endPos);
  
  if (endPos < 0) endPos = ifs.size();
  
//  int endPos = -1;
//  if (scanSize >= 0) {
//    endPos = initialOffset + scanSize;
//  }
  
/*  while (!ifs.eof()) {
    StringCheckResult result = checkString(ifs);
    if (result.charCount < minimumStringLen) {
      ifs.get();
      continue;
    }
    
    string str(ifs.data().data() + ifs.tell());
    cout << hex << setw(6) << ifs.tell() << "|" << str << endl;
    ifs.seekoff(result.rawLength);
  }*/
  
  ifs.seek(initialOffset);
  int lastRep = initialOffset;
  int toNextRep = 0x800;
  while (!ifs.eof()) {
    // check if limit reached
    if (ifs.tell() >= endPos) break;
    
    if ((ifs.tell() - lastRep) >= toNextRep) {
      std::cerr << "now at: " << std::hex << ifs.tell() << std::endl;
      lastRep = (lastRep + toNextRep);
    }
    
    StringCheckResult result = checkString(ifs, table, terminator);
    if (result.charCount < minimumStringLen) {
      ifs.get();
      continue;
    }
    
    cout << hex << setw(8) << ifs.tell() << "|" << result.str << endl;
    ifs.seekoff(result.rawLength);
  }
  
  
  return 0;
}
