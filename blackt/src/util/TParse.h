#ifndef TPARSE_H
#define TPARSE_H


#include "util/TStream.h"
#include <string>

namespace BlackT {

class TParse {
public:
  static void skipSpace(BlackT::TStream& ifs);
  static bool checkString(BlackT::TStream& ifs);
  static bool checkInt(BlackT::TStream& ifs);
  static bool checkChar(BlackT::TStream& ifs, char c);
  static std::string matchString(BlackT::TStream& ifs);
  static int matchInt(BlackT::TStream& ifs);
  static double matchDouble(BlackT::TStream& ifs);
  static void matchChar(BlackT::TStream& ifs, char c);
  static std::string matchName(BlackT::TStream& ifs);
  
  static std::string getSpacedSymbol(BlackT::TStream& ifs);
  static std::string getRemainingContent(BlackT::TStream& ifs);
  
  static std::string getUntilChars(BlackT::TStream& ifs,
                                   std::string recstring);
  static std::string getUntilSpaceOrChars(BlackT::TStream& ifs,
                                          std::string recstring);

};

}


#endif
