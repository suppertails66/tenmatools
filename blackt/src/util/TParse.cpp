#include "util/TParse.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <cctype>
#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>

namespace BlackT {



void TParse::skipSpace(BlackT::TStream& ifs) {
  ifs.skipSpace();
}

bool TParse::checkString(BlackT::TStream& ifs) {
  skipSpace(ifs);
  
  if (!ifs.eof() && (ifs.peek() == '"')) return true;
  return false;
}

bool TParse::checkInt(BlackT::TStream& ifs) {
  skipSpace(ifs);
  
  if (!ifs.eof()
      && (isdigit(ifs.peek()) || (ifs.peek() == '$')
          || (ifs.peek() == '-') || (ifs.peek() == '+')))
      return true;
  return false;
}

bool TParse::checkChar(BlackT::TStream& ifs, char c) {
  skipSpace(ifs);
  
  if (!ifs.eof() && (ifs.peek() == c)) return true;
  return false;
}

std::string TParse::matchString(BlackT::TStream& ifs) {
  skipSpace(ifs);
  if (ifs.eof()) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchString()",
                            "Unexpected end of line");
  }
  
  if (!checkString(ifs)) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchString()",
                             std::string("Unexpected non-string at: ")
                              + getRemainingContent(ifs));
  }
  
  ifs.get();
  
  std::string result;
  while (!ifs.eof() && (ifs.peek() != '"')) result += ifs.get();
  
  matchChar(ifs, '"');
  
  return result;
}

int TParse::matchInt(BlackT::TStream& ifs) {
  skipSpace(ifs);
  if (ifs.eof()) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchInt()",
                            "Unexpected end of line");
  }
  
  if (!checkInt(ifs)) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchInt()",
                            std::string("Unexpected non-int at: ")
                              + getRemainingContent(ifs));
  }
  
  std::string numstr;
  // get unary +/- if it exists
  if ((ifs.peek() == '+') || (ifs.peek() == '-')) numstr += ifs.get();
  // get first character (this covers the case of an initial '$' for
  // hexadecimal)
  numstr += ifs.get();
  // handle possible initial "0x"
  if ((numstr[0] == '0') && (tolower(ifs.peek()) == 'x')) numstr += ifs.get();
  
  char next = ifs.peek();
  while (!ifs.eof()
         && (isdigit(next)
          || (tolower(next) == 'a')
          || (tolower(next) == 'b')
          || (tolower(next) == 'c')
          || (tolower(next) == 'd')
          || (tolower(next) == 'e')
          || (tolower(next) == 'f'))) {
    numstr += ifs.get();
    next = ifs.peek();
  }
  
  return TStringConversion::stringToInt(numstr);
}

double TParse::matchDouble(BlackT::TStream& ifs) {
  skipSpace(ifs);
  if (ifs.eof()) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchDouble()",
                            "Unexpected end of line");
  }
  
  // double must be at least preceded by integer
  if (!checkInt(ifs)) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchDouble()",
                            std::string("Unexpected non-double at: ")
                              + getRemainingContent(ifs));
  }
  
  std::string numstr;
  // get unary +/- if it exists
  if ((ifs.peek() == '+') || (ifs.peek() == '-')) numstr += ifs.get();
  // get first character (this covers the case of an initial '$' for
  // hexadecimal)
  numstr += ifs.get();
  // handle possible initial "0x"
  if ((numstr[0] == '0') && (tolower(ifs.peek()) == 'x')) numstr += ifs.get();
  
  char next = ifs.peek();
  bool periodFound = false;
  while (!ifs.eof()
         && (isdigit(next)
             || (next == '.'))) {
    if (!periodFound && (next == '.')) periodFound = true;
    else if (periodFound && (next == '.')) {
      throw TGenericException(T_SRCANDLINE,
                              "TParse::matchDouble()",
                              std::string("Multiple periods in double at: ")
                                + getRemainingContent(ifs));
    }
    
    numstr += ifs.get();
    next = ifs.peek();
  }
  
  double result;
  sscanf(numstr.c_str(), "%lf", &result);
  return result;
}

void TParse::matchChar(BlackT::TStream& ifs, char c) {
  skipSpace(ifs);
  if (ifs.eof()) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchChar()",
                            "Unexpected end of line");
  }
  
  if (ifs.peek() != c) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchChar()",
                            std::string("Expected '")
                              + c
                              + "', got '"
                              + ifs.get()
                              + "'");
  }
  
  ifs.get();
}

std::string TParse::matchName(BlackT::TStream& ifs) {
  skipSpace(ifs);
  if (ifs.eof()) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchName()",
                            "Unexpected end of line");
  }
  
  if (!isalpha(ifs.peek())) {
    throw TGenericException(T_SRCANDLINE,
                            "TParse::matchName()",
                            std::string("Couldn't read name at: ")
                              + getRemainingContent(ifs));
  }
  
  std::string result;
  result += ifs.get();
  while (!ifs.eof()
         && (isalnum(ifs.peek()) || (ifs.peek() == '_'))) {
    result += ifs.get();
  }
  
  return result;
}

std::string TParse::getSpacedSymbol(BlackT::TStream& ifs) {
  skipSpace(ifs);
  if (ifs.eof()) {
//    throw TGenericException(T_SRCANDLINE,
//                            "TParse::getSpacedSymbol()",
//                            "Unexpected end of line");
    return "";
  }
  
  std::string result;
  result += ifs.get();
  while (!ifs.eof()
         && (!isspace(ifs.peek()))) {
    result += ifs.get();
  }
  
  return result;
}

std::string TParse
  ::getRemainingContent(BlackT::TStream& ifs) {
  std::string content;
  while (!ifs.eof()) content += ifs.get();
  return content;
}

std::string TParse::getUntilChars(BlackT::TStream& ifs,
                                 std::string recstring) {
  std::string result;
  
  while (!ifs.eof()) {
    char next = ifs.get();
    bool match = false;
    for (int i = 0; i < recstring.size(); i++) {
      if (recstring[i] == next) {
        match = true;
        break;
      }
    }
    
    if (match) {
      break;
    }
    
    result += next;
  }
  
  return result;
}

std::string TParse::getUntilSpaceOrChars(BlackT::TStream& ifs,
                                         std::string recstring) {
  std::string result;
  
  while (!ifs.eof()) {
    char next = ifs.get();
    bool match = false;
    for (int i = 0; i < recstring.size(); i++) {
      if (isspace(next) || (recstring[i] == next)) {
        match = true;
        break;
      }
    }
    
    if (match) {
      break;
    }
    
    result += next;
  }
  
  return result;
}


}
