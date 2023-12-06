#ifndef TSJIS_H
#define TSJIS_H


#include <string>
#include "util/TStream.h"

namespace BlackT {


class TSjis {
public:
  struct SjisCheckResult {
    int rawLength;
    int charCount;
  };
  
  static bool isSjis(int code);
  
  // if the next byte(s) in ifs form a SJIS character, this returns the
  // size in bytes of that character; otherwise, it returns a negative value
  static int checkSjisChar(TStream& ifs);
  
  // returns SJIS char at ifs, or -1 if none
  static int fetchSjisChar(TStream& ifs);
  
  // returns length of null-terminated SJIS string (including terminator) in
  // ifs if a valid one exists, or -1 if invalid.
  // does not advance ifs.
  static SjisCheckResult checkSjisString(TStream& ifs);
  
  static bool is2bSjis(int code);
protected:
  static bool between(int value, int lower, int upper);
};


};


#endif
