#include "smpce/SmPceScriptOps.h"
#include "smpce/SmPceVarScriptDecmp.h"
#include "smpce/SmPceMsgScriptDecmp.h"
#include "smpce/SmPceVarScriptCmp.h"
#include "smpce/SmPceMsgScriptCmp.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace BlackT;

namespace Pce {

std::map<std::string, bool> scriptStrings;
std::string lastScriptString;

std::map<int, std::string> msgNumToSoundFile;
std::map<std::string, bool> soundFileNames;

std::map<std::string, bool> loadedFileNames;
std::map<std::string, bool> loaded2FileNames;

std::vector<std::string> scriptMessages;
std::string activeMessage;

/*std::string prepReadString(const std::string& input) {
  std::string output;
  int i = 0;
  while (i < input.size()) {
    if ((unsigned char)input[i] < 0x80) {
      output += input[i];
      ++i;
    }
    else {
      // SJIS percent sign == linebreak
      if (((unsigned char)input[i] == 0x81)
          && ((unsigned char)input[i + 1] == 0x93)) {
        output += '\n';
      }
      else {
        output += input[i];
        output += input[i + 1];
      }
      
      i += 2;
    }
  }
  
  return output;
} */

std::string prepReadString(const std::string& input,
                           std::string& label,
                           std::string& output) {
  std::string temp;
  int i = 0;
  while (i < input.size()) {
    if ((unsigned char)input[i] < 0x80) {
      temp += input[i];
      ++i;
    }
    else {
      // SJIS percent sign == linebreak
      if (((unsigned char)input[i] == 0x81)
          && ((unsigned char)input[i + 1] == 0x93)) {
        temp += '\n';
      }
      else {
        temp += input[i];
        temp += input[i + 1];
      }
      
      i += 2;
    }
  }
  
//  std::string label;
//  std::string output;
  i = 0;
  
  // SJIS left parenthesis at message start == possible label
  bool hasLabel = false;
  if (((unsigned char)temp[0] == 0x81)
      && ((unsigned char)temp[1] == 0x6B)) {
    // terminator: SJIS right parenthesis followed by newline,
    // if this sequence occurs on the first line of the input
    while (i < temp.size() - 3) {
      if (temp[i] == '\n') break;
      else if (((unsigned char)temp[i] == 0x81)
          && ((unsigned char)temp[i + 1] == 0x6C)
          && ((char)temp[i + 2] == '\n')) {
        hasLabel = true;
        i += 3;
        label = temp.substr(0, i - 1);
        break;
      }
      else {
        if ((unsigned char)temp[i] < 0x80) ++i;
        else i += 2;
      }
    }
  }
  
  if (!hasLabel) i = 0;
  
  output = temp.substr(i, std::string::npos);
  
//  return output;
  return std::string("\"") + label + "\",\"" + output + "\"";
}

void skipSpace(std::istream& ifs, int& lineNum) {
  while (true) {
    char next = ifs.peek();
    
    // ignore spaces
    if (isspace(next)) {
      if (next == '\n') ++lineNum;
      ifs.get();
    }
    // ignore comment lines
    else if (next == '/') {
      ifs.get();
      next = ifs.peek();
      if (next == '/') {
        ifs.get();
        while (ifs.peek() != '\n') ifs.get();
        ifs.get();
        ++lineNum;
      }
      // block comment
      else if (next == '*') {
        ifs.get();
        while (true) {
          if (ifs.peek() == '\n') ++lineNum;
          
          if (ifs.peek() == '*') {
            ifs.get();
            if (ifs.peek() == '/') {
              ifs.get();
              break;
            }
          }
          else {
            ifs.get();
          }
        }
      }
      else {
        // done
        ifs.unget();
        break;
      }
    }
    // anything else = done
    else {
      break;
    }
  }
}

std::string getSpacedSymbol(std::istream& ifs, int& lineNum) {
  skipSpace(ifs, lineNum);
  std::string result;
  while (!isspace(ifs.peek())) result += ifs.get();
  return result;
}

int getInt(std::istream& ifs, int& lineNum) {
  std::string str = getSpacedSymbol(ifs, lineNum);
  return TStringConversion::stringToInt(str);
}

bool checkSpacedSymbol(std::istream& ifs, std::string sym, int& lineNum) {
  int pos = ifs.tellg();
  std::string str = getSpacedSymbol(ifs, lineNum);
  ifs.seekg(pos);
  if (sym.compare(str) == 0) return true;
  else return false;
}

void matchSpacedSymbol(std::istream& ifs, std::string sym, int& lineNum) {
  std::string str = getSpacedSymbol(ifs, lineNum);
  if (str.compare(sym) != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "matchSpacedSymbol()",
                            std::string("Line ")
                              + TStringConversion::intToString(lineNum)
                              + ":\n  "
                              + "Couldn't match symbol '"
                              + sym
                              + "' at '"
                              + str
                              + "'");
  }
}

bool checkChar(std::istream& ifs, char sym, int& lineNum) {
  skipSpace(ifs, lineNum);
  if (ifs.peek() == sym) {
    return true;
  }
  
  return false;
}

void matchChar(std::istream& ifs, char sym, int& lineNum) {
  if (!checkChar(ifs, sym, lineNum)) {
    throw TGenericException(T_SRCANDLINE,
                            "matchSpacedSymbol()",
                            std::string("Line ")
                              + TStringConversion::intToString(lineNum)
                              + ":\n  "
                              + "Couldn't match symbol '"
                              + sym
                              + "'");
  }
  
  ifs.get();
}

//*****************************************************************************
// Generic
//*****************************************************************************

SmPceScriptOp* getScriptOp(SmPceScriptOp* ops, int numOps, int id) {
  for (int i = 0; i < numOps; i++) {
    SmPceScriptOp* op = &(ops[i]);
    if ((id >= op->idLow) && (id <= op->idHigh)) return op;
  }
  
  return NULL;
}

SmPceScriptOp* getScriptOp(SmPceScriptOp* ops, int numOps, std::string name) {
  for (int i = 0; i < numOps; i++) {
    SmPceScriptOp* op = &(ops[i]);
    if (strcmp(name.c_str(), op->name) == 0) return op;
  }
  
  return NULL;
}

void opIndent(std::ostream& ofs, int indentLevel) {
  for (int i = 0; i < indentLevel; i++) {
    ofs << "  ";
  }
}

void printOpName(std::ostream& ofs, const char* name) {
  int len = std::strlen(name);
  int remaining = 15 - len;
  if (remaining < 0) remaining = 0;
  
  ofs << name;
  for (int i = 0; i < remaining; i++) ofs << " ";
  ofs << " ";
}

std::string toHex(int value) {
  return std::string("$")
          + TStringConversion::intToString(value,
            TStringConversion::baseHex).substr(2, std::string::npos);
}

int fromHex(std::string str) {
  return TStringConversion::stringToInt(str);
}

void printOpStart(std::ostream& ofs, const char* opName, int indentLevel) {
  opIndent(ofs, indentLevel);
  printOpName(ofs, opName);
}

void writeOpStart(BlackT::TStream& ofs,
                  int id,
                  int& lineNum) {
//  opIndent(ofs, indentLevel);
//  printOpName(ofs, opName);
  ofs.writeu8(id);
}

void readSingleGeneric(SmPceScriptOp* op,
                       int id,
                       BlackT::TStream& ifs, std::ostream& ofs,
                       int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
}

void writeSingleGeneric(SmPceScriptOp* op,
                       std::istream& ifs, BlackT::TStream& ofs,
                       int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
}

void readString(BlackT::TStream& ifs, std::ostream& ofs,
               int indentLevel, CharacterDictionary* dic) {
  std::string str;
  
  char next = ifs.get();
  while (next != 0x06) {
    str += next;
    next = ifs.get();
  }
  
  ofs << "\"" << str << "\"";
  
  // ****************
  for (unsigned int i = 0; i < str.size(); i++) {
    str[i] = tolower(str[i]);
  }
  if (scriptStrings.find(str) == scriptStrings.end()) {
    scriptStrings[str] = false;
  }
  lastScriptString = str;
}

void writeString(std::istream& ifs, BlackT::TStream& ofs,
                 int& lineNum, MessageDictionary* dic) {
  std::string str;
  skipSpace(ifs, lineNum);
  matchChar(ifs, '"', lineNum);
  while (ifs.peek() != '"') str += ifs.get();
  matchChar(ifs, '"', lineNum);
                 
  // needed?
  ofs.writeu8(0x06);
  
  for (unsigned int i = 0; i < str.size(); i++) {
    ofs.put(str[i]);
  }
  ofs.writeu8(0x06);
}

void readStringGeneric(SmPceScriptOp* op,
                       int id,
                       BlackT::TStream& ifs, std::ostream& ofs,
                       int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  readString(ifs, ofs, indentLevel, dic);
  
  ofs << std::endl;
}

void writeStringGeneric(SmPceScriptOp* op,
                        std::istream& ifs, BlackT::TStream& ofs,
                        int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  writeString(ifs, ofs, lineNum, dic);
}

bool readLocal(BlackT::TStream& ifs, std::ostream& ofs,
               int indentLevel, CharacterDictionary* dic) {
  int value = ifs.readu8();
  
  // 01 == script
  if (value == 0x01) {
    SmPceMsgScriptDecmp(ifs, ofs, indentLevel, dic)();
  }
  // 06 == string
  else if (value == 0x06) {
    opIndent(ofs, indentLevel);
    readString(ifs, ofs, indentLevel, dic);
    ofs << std::endl;
  }
  else {
    // if 07, 08, 09, or 20-5A inclusive, varscript
    if ((value == 0x07) || (value == 0x08) || (value == 0x09)
        || ((value >= 0x20) && (value <= 0x5A))) {
      ifs.unget();
      SmPceVarScriptDecmp(ifs, ofs, indentLevel, dic)();
    }
    // otherwise, nothing (i.e. no locals)
    else {
      ifs.unget();
      return false;
    }
  }
  
  return true;
}

void writeLocal(std::istream& ifs, BlackT::TStream& ofs,
                int& lineNum, MessageDictionary* dic) {
  skipSpace(ifs, lineNum);
  
  // { == script
  if (ifs.peek() == '{') {
    ofs.writeu8(0x01);
    SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
  }
  // " == string
  else if (ifs.peek() == '"') {
    writeString(ifs, ofs, lineNum, dic);
  }
  else {
    // if [, varscript
    if (ifs.peek() == '[') {
      SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
    }
    // otherwise, nothing (i.e. no locals)
    else {
      //matchChar(ifs, ')', lineNum);
    }
  }
}

void readLocals(BlackT::TStream& ifs, std::ostream& ofs,
               int indentLevel, CharacterDictionary* dic) {
  opIndent(ofs, indentLevel);
  ofs << "(" << std::endl;
  
  while (true) {
    bool result = readLocal(ifs, ofs, indentLevel + 1, dic);
    if (!result) break;
    
    char next = ifs.peek();
    if (next != 0x02) break;
    else ifs.get();
  }
  
  opIndent(ofs, indentLevel);
  ofs << ")" << std::endl;
}

void writeLocals(std::istream& ifs, BlackT::TStream& ofs,
                 int& lineNum, MessageDictionary* dic) {
  matchChar(ifs, '(', lineNum);
  
  while (true) {
    // write next arg (possibly none)
    writeLocal(ifs, ofs, lineNum, dic);
    
    // break at end of args
    if (checkChar(ifs, ')', lineNum)) break;
    // otherwise, write more-args token
    else ofs.writeu8(0x02);
  }
  
  matchChar(ifs, ')', lineNum);
}

int fetchVarLiteral(BlackT::TStream& ifs) {
  int type = ifs.readu8();
  
  int value = 0;
  switch (type) {
  case 9:
    value |= (ifs.readu8() & 0xFC) << 16;
    value >>= 2;
    // ! drop through
  case 8:
    value |= (ifs.readu8() & 0xFC) << 8;
    value >>= 2;
    // ! drop through
  case 7:
    value |= (ifs.readu8() & 0xFC);
    value >>= 2;
    break;
  default:
    if ((type >= 0x30) && (type <= 0x3F)) {
      value = (type - 0x30);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "fetchVarLiteral()",
                              std::string("Unknown var literal type ")
                                + TStringConversion::intToString(type,
                                    TStringConversion::baseHex));
    }
    break;
  }
  
  value &= 0xFFFF;
  return value;
}

void writeVarLiteral(std::istream& ifs, BlackT::TStream& ofs,
                     int& lineNum, MessageDictionary* dic,
                     int type) {
  ofs.writeu8(type);
  
  if ((type >= 0x30) && (type <= 0x3F)) {
    return;
  }
    
  int value = getInt(ifs, lineNum);
  switch (type) {
  case 9:
  {
    int sub = (value & 0x03F000) >> 12;
    sub <<= 2;
    sub |= 0x03;
    ofs.writeu8(sub);
    
//    value |= (ifs.readu8() & 0xFC) << 16;
//    value >>= 2;
  }
    // ! drop through
  case 8:
  {
    int sub = (value & 0x000FC0) >> 6;
    sub <<= 2;
    sub |= 0x03;
    ofs.writeu8(sub);
    
//    value |= (ifs.readu8() & 0xFC) << 8;
//    value >>= 2;
  }
    // ! drop through
  case 7:
  {
    int sub = (value & 0x00003F);
    sub <<= 2;
    sub |= 0x03;
    ofs.writeu8(sub);
    
//    value |= (ifs.readu8() & 0xFC);
//    value >>= 2;
  }
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "writeVarLiteral()",
                            std::string("Unknown var literal type ")
                              + TStringConversion::intToString(type,
                                  TStringConversion::baseHex));
    break;
  }
}

//*****************************************************************************
// Var scripts
//*****************************************************************************

/*void readPushShort(SmPceScriptOp* op, int id,
                   BlackT::TStream& ifs, std::ostream& ofs,
                   int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  ifs.unget();
  int value = fetchVarLiteral(ifs);
  
  ofs << toHex(value);
  
  ofs << std::endl;
}

void readPushInt(SmPceScriptOp* op, int id,
                 BlackT::TStream& ifs, std::ostream& ofs,
                 int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  ifs.unget();
  int value = fetchVarLiteral(ifs);
  
  ofs << toHex(value);
  
  ofs << std::endl;
}

void readPushLong(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  ifs.unget();
  int value = fetchVarLiteral(ifs);
  
  ofs << toHex(value);
  
  ofs << std::endl;
} */

void readPushIntGeneric(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  ifs.unget();
  int value = fetchVarLiteral(ifs);
  
  ofs << toHex(value);
  
  ofs << std::endl;
}

void writePushIntGeneric(SmPceScriptOp* op,
                    std::istream& ifs, BlackT::TStream& ofs,
                    int& lineNum, MessageDictionary* dic) {
  if ((strcmp(op->name, "pushImmed")) == 0) {
    writeVarLiteral(ifs, ofs, lineNum, dic,
                    fromHex(getSpacedSymbol(ifs, lineNum)) + 0x30);
  }
  else {
    writeVarLiteral(ifs, ofs, lineNum, dic, op->idLow);
  }
}

/*const char* varMathSubopNames[] = {
  "add",
  "sub",
  "mul",
  "div",
  "rem",
  "or",
  "and",
  "eq",
  "neg",
  "lt",
  "gt",
  "readWordArray",
  "readWordArrayByte",
  "litNybArrayRead",
  "readNybArray",
  "rand"
}; */

void readVarMath(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
                  
  printOpStart(ofs, op->name, indentLevel);
//  opIndent(ofs, indentLevel);
  
//  int subop = id - 0x20;
//  printOpName(ofs, varMathSubopNames[subop]);
  
  if ((id == 0x2D) || (id == 0x2F)) {
    int type = (unsigned char)ifs.peek();
    int value = fetchVarLiteral(ifs);
    
    switch (type) {
    case 7:
      ofs << "short ";
      break;
    case 8:
      ofs << "int ";
      break;
    case 9:
      ofs << "long ";
      break;
    default:
      if ((type >= 0x30) && (type <= 0x3F)) {
        ofs << "immed ";
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "readVarMath()",
                                std::string("Unknown var literal type ")
                                  + TStringConversion::intToString(type,
                                      TStringConversion::baseHex));
      }
      break;
    }
    
    ofs << toHex(value);
  }
  
//  ofs << varMathNames[subop];
//  ofs << toHex(value);
  
  ofs << std::endl;
}

void writeVarMath(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
                  
//  printOpStart(ofs, op->name, indentLevel);
//  opIndent(ofs, indentLevel);
  writeOpStart(ofs, op->idLow, lineNum);
  
//  int subop = id - 0x20;
//  printOpName(ofs, varMathSubopNames[subop]);
  
  if ((op->idLow == 0x2D) || (op->idLow == 0x2F)) {
//    int type = (unsigned char)ifs.peek();
//    int value = fetchVarLiteral(ifs);
    std::string argname = getSpacedSymbol(ifs, lineNum);
    
    if (argname == "short") {
      writeVarLiteral(ifs, ofs, lineNum, dic, 7);
    }
    else if (argname == "int") {
      writeVarLiteral(ifs, ofs, lineNum, dic, 8);
    }
    else if (argname == "long") {
      writeVarLiteral(ifs, ofs, lineNum, dic, 9);
    }
    else if (argname == "immed") {
      int type = getInt(ifs, lineNum) + 0x30;
      writeVarLiteral(ifs, ofs, lineNum, dic, type);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "writeVarMath()",
                              std::string("Unknown var literal type '")
                                + argname
                                + "'");
    }
  }
  
//  ofs << varMathNames[subop];
//  ofs << toHex(value);
  
//  ofs << std::endl;
}

void readPushImmed(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  int value = id - 0x30;
  
  ofs << toHex(value);
  
  ofs << std::endl;
}

void writePushImmed(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  int type = getInt(ifs, lineNum) + 0x30;
  writeOpStart(ofs, type, lineNum);
}

void readPushArrayWord(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  int value = id - 0x40;
  
  ofs << toHex(value);
  
  ofs << std::endl;
}

void writePushArrayWord(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  int type = getInt(ifs, lineNum) + 0x40;
  writeOpStart(ofs, type, lineNum);
}

SmPceScriptOp smPceVarScriptOps[] = {
  { 0x03, 0x03, "retVar", readSingleGeneric, writeSingleGeneric },
  { 0x07, 0x07, "pushShort", readPushIntGeneric, writePushIntGeneric },
  { 0x08, 0x08, "pushInt", readPushIntGeneric, writePushIntGeneric },
  { 0x09, 0x09, "pushLong", readPushIntGeneric, writePushIntGeneric },
//  { 0x20, 0x2F, "varMath", readVarMath },
  { 0x20, 0x20, "add", readVarMath, writeVarMath },
  { 0x21, 0x21, "sub", readVarMath, writeVarMath },
  { 0x22, 0x22, "mul", readVarMath, writeVarMath },
  { 0x23, 0x23, "div", readVarMath, writeVarMath },
  { 0x24, 0x24, "rem", readVarMath, writeVarMath },
  { 0x25, 0x25, "or", readVarMath, writeVarMath },
  { 0x26, 0x26, "and", readVarMath, writeVarMath },
  { 0x27, 0x27, "eq", readVarMath, writeVarMath },
  { 0x28, 0x28, "neg", readVarMath, writeVarMath },
  { 0x29, 0x29, "lt", readVarMath, writeVarMath },
  { 0x2A, 0x2A, "gt", readVarMath, writeVarMath },
  { 0x2B, 0x2B, "readWordArray", readVarMath, writeVarMath },
  { 0x2C, 0x2C, "readWordArrayByte", readVarMath, writeVarMath },
  { 0x2D, 0x2D, "litNybArrayRead", readVarMath, writeVarMath },
  { 0x2E, 0x2E, "readNybArray", readVarMath, writeVarMath },
  { 0x2F, 0x2F, "rand", readVarMath, writeVarMath },
  { 0x30, 0x3F, "pushImmed", readPushImmed, writePushImmed },
  { 0x40, 0x5A, "pushArrayWord", readPushArrayWord, writePushArrayWord }
};

/*SmPceScriptOp smPceVarScriptVarMathOps[] = {
  { 0x20, 0x20, "add", readVarMath },
  { 0x21, 0x21, "sub", readVarMath },
  { 0x22, 0x22, "mul", readVarMath },
  { 0x23, 0x23, "div", readVarMath },
  { 0x24, 0x24, "rem", readVarMath },
  { 0x25, 0x25, "or", readVarMath },
  { 0x26, 0x26, "and", readVarMath },
  { 0x27, 0x27, "eq", readVarMath },
  { 0x28, 0x28, "neg", readVarMath },
  { 0x29, 0x29, "lt", readVarMath },
  { 0x2A, 0x2A, "gt", readVarMath },
  { 0x2B, 0x2B, "readWordArray", readVarMath },
  { 0x2C, 0x2C, "readWordArrayByte", readVarMath },
  { 0x2D, 0x2D, "litNybArrayRead", readVarMath },
  { 0x2E, 0x2E, "readNybArray", readVarMath },
  { 0x2F, 0x2F, "rand", readVarMath }
}; */

//*****************************************************************************
// Message scripts
//*****************************************************************************

void readVarThenMsgGeneric(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  SmPceVarScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
//  int value = id - 0x40;
  
//  ofs << toHex(value);
  
}

void writeVarThenMsgGeneric(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
//  printOpStart(ofs, op->name, indentLevel);
//  ofs << std::endl;
  writeOpStart(ofs, op->idLow, lineNum);
  
  SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
  SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
  
//  int value = id - 0x40;
  
//  ofs << toHex(value);
  
}

void readMsgGeneric(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
//  int value = id - 0x40;
  
//  ofs << toHex(value);
  
}

void writeMsgGeneric(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
}

void readLocalsGeneric(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  readLocals(ifs, ofs, indentLevel, dic);
  
//  SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
//  int value = id - 0x40;
  
//  ofs << toHex(value);
  
}

void writeLocalsGeneric(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  writeLocals(ifs, ofs, lineNum, dic);
}

/*void readOp16(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  readLocals(ifs, ofs, indentLevel, dic);
  
//  if (ifs.peek() == 0x02) ifs.get();
  
  
//  SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
//  int value = id - 0x40;
  
//  ofs << toHex(value);
  
} */

void readIfThenWhile(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  // ??? ignored???
  int value = ifs.readu8();
  ofs << toHex(value);
  ofs << std::endl;
  
  
  SmPceVarScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
  // skip "01" -- this is not actually an embedded call
  ifs.get();
  SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
//  int value = id - 0x40;
  
//  ofs << toHex(value);
  
}

void writeIfThenWhile(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  // ??? ignored???
  int value = getInt(ifs, lineNum);
  ofs.writeu8(value);
  
  SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
  
  // add "01" before subscript
  ofs.writeu8(0x01);
  SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
}

void readFuncOp13(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  opIndent(ofs, indentLevel);
  ofs << "(" << std::endl;
  
  // skip "01"
  ifs.get();
  SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
  ifs.unget();
  while (ifs.get() == 0x02) {
    SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
    ifs.unget();
  }
  
  opIndent(ofs, indentLevel);
  ofs << ")" << std::endl;
}

void writeFuncOp13(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  matchChar(ifs, '(', lineNum);
  
  ofs.writeu8(0x01);
  SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
  
  while (checkChar(ifs, '{', lineNum)) {
    SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
  }
  
  matchChar(ifs, ')', lineNum);
}

void readCallImmed(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  // Decompile the embedded script
  SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
//  int value = id - 0x40;
  
//  ofs << toHex(value);
  
}

void writeCallImmed(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  // Compile the embedded script
  SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
}

void readWriteNybblesLiteral(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);

  int type = (unsigned char)ifs.peek();
  int value = fetchVarLiteral(ifs);
  
  switch (type) {
  case 7:
    ofs << "short ";
    break;
  case 8:
    ofs << "int ";
    break;
  case 9:
    ofs << "long ";
    break;
  default:
    if ((type >= 0x30) && (type <= 0x3F)) {
      ofs << "immed ";
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "readWriteNybblesLiteral()",
                              std::string("Unknown var literal type ")
                                + TStringConversion::intToString(type,
                                    TStringConversion::baseHex));
    }
    break;
  }
  ofs << toHex(value);
  ofs << std::endl;
  
  opIndent(ofs, indentLevel);
  ofs << "(" << std::endl;
  
  while (true) {
    
    SmPceVarScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
    
    char next = ifs.peek();
    if (next != 0x02) break;
    else ifs.get();
  }
  
  opIndent(ofs, indentLevel);
  ofs << ")" << std::endl;
}

void writeWriteNybblesLiteral(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  std::string argname = getSpacedSymbol(ifs, lineNum);
  if (argname == "short") {
    writeVarLiteral(ifs, ofs, lineNum, dic, 7);
  }
  else if (argname == "int") {
    writeVarLiteral(ifs, ofs, lineNum, dic, 8);
  }
  else if (argname == "long") {
    writeVarLiteral(ifs, ofs, lineNum, dic, 9);
  }
  else if (argname == "immed") {
    int type = getInt(ifs, lineNum) + 0x30;
    writeVarLiteral(ifs, ofs, lineNum, dic, type);
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "writeWriteNybblesLiteral()",
                            std::string("Unknown var literal type '")
                              + argname
                              + "'");
  }
  
  matchChar(ifs, '(', lineNum);
  
  while (true) {
    SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
    
    if (checkChar(ifs, ')', lineNum)) {
      break;
    }
    else {
      ofs.writeu8(0x02);
    }
  }
  
  matchChar(ifs, ')', lineNum);
}

void readWriteNybbles(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  SmPceVarScriptDecmp(ifs, ofs, indentLevel, dic)();
  
  opIndent(ofs, indentLevel);
  ofs << "(" << std::endl;
  
  while (true) {
    
    SmPceVarScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
    
    char next = ifs.peek();
    if (next != 0x02) break;
    else ifs.get();
  }
  
  opIndent(ofs, indentLevel);
  ofs << ")" << std::endl;
}

void writeWriteNybbles(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
  
  matchChar(ifs, '(', lineNum);
  
  while (true) {
    SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
    
    if (checkChar(ifs, ')', lineNum)) {
      break;
    }
    else {
      ofs.writeu8(0x02);
    }
  }
  
  matchChar(ifs, ')', lineNum);
}

void readWriteValueWord(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  int value = ifs.readu8() - 0x40;
  ofs << toHex(value) << std::endl;
  
  SmPceVarScriptDecmp(ifs, ofs, indentLevel, dic)();
}

void writeWriteValueWord(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);

  int type = getInt(ifs, lineNum) + 0x40;
  ofs.writeu8(type);

  SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
}

void readWriteArrayWords(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  
  int value = ifs.readu8() - 0x40;
  ofs << toHex(value) << std::endl;
  
  SmPceVarScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  
  opIndent(ofs, indentLevel);
  ofs << "(" << std::endl;
  
  while (true) {
    
    SmPceVarScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
    
    char next = ifs.peek();
    if (next != 0x02) break;
    else ifs.get();
  }
  
  opIndent(ofs, indentLevel);
  ofs << ")" << std::endl;
}

void writeWriteArrayWords(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);

  int type = getInt(ifs, lineNum) + 0x40;
  ofs.writeu8(type);

  SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
  
  matchChar(ifs, '(', lineNum);
  
  while (true) {
    SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
    
    if (checkChar(ifs, ')', lineNum)) {
      break;
    }
    else {
      ofs.writeu8(0x02);
    }
  }
  
  matchChar(ifs, ')', lineNum);
}

void readIf(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  printOpStart(ofs, op->name, indentLevel);
  ofs << std::endl;
  
  SmPceVarScriptDecmp(ifs, ofs, indentLevel, dic)();
  
//  opIndent(ofs, indentLevel);
//  ofs << "{" << std::endl;

  // if branch -- must exist
  ifs.get();
  SmPceMsgScriptDecmp(ifs, ofs, indentLevel, dic)();
  
  // if "02" after if branch, there is an else branch
  char next = ifs.peek();
  if (next == 0x02) {
    ifs.get();
    printOpStart(ofs, "else", indentLevel);
    ofs << std::endl;
//    SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
  }
  else {
    
  }

  
//  for (int i = 0; i < 2; i++) {
/*    while (true) {
      
      // skip "01" -- this is not actually an embedded call
      ifs.get();
      
      SmPceMsgScriptDecmp(ifs, ofs, indentLevel + 1, dic)();
      
      char next = ifs.peek();
      if (next != 0x02) break;
      else {
        // what actually happens here is that, I guess to avoid growing the
        // stack too much, the logic for the "false" branch of the if
        // will check if the next op is 0F (another if) and manually jump
        // back to the start of the "if" logic if so.
        // in any other case, the "false" script will be executed normally.
        // whatever happens, there is an extraneous(?) byte here.
        ifs.get();
        break;
  //      std::cerr << ifs.tell() << std::endl;
      }
    }
//  } */
  
//  opIndent(ofs, indentLevel);
//  ofs << "}" << std::endl;
}

void writeIf(SmPceScriptOp* op,
                  std::istream& ifs, BlackT::TStream& ofs,
                  int& lineNum, MessageDictionary* dic) {
  writeOpStart(ofs, op->idLow, lineNum);
  
  SmPceVarScriptCmp(ifs, ofs, lineNum, dic)();
  
//  opIndent(ofs, indentLevel);
//  ofs << "{" << std::endl;

  // if branch -- must exist
  ofs.writeu8(0x01);
  SmPceMsgScriptCmp(ifs, ofs, lineNum, dic)();
  
  // "else" is specially handled -- ignore here
}

void readSjisChar(int codepoint, std::ostream& ofs,
                  int indentLevel) {
  unsigned char first = (unsigned char)((codepoint & 0xFF00) >> 8);
  unsigned char second = (unsigned char)((codepoint & 0x00FF));
  
  if (activeMessage.empty()) {
    opIndent(ofs, indentLevel);
    ofs << "<MSG_" << scriptMessages.size() << "> // ";
  }
  
  ofs.put(first);
  ofs.put(second);
  
  activeMessage += first;
  activeMessage += second;
}

void readSjisLiteral(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
//  printOpStart(ofs, op->name, indentLevel);
//  ofs << std::endl;
  
  // high byte is 0x20 less than actual value
//  ofs.put(((unsigned char)id) + 0x20);
//  ofs.put(((unsigned char)ifs.get()) + 0x00);
  
  int codepoint = ((unsigned char)id) + 0x20;
  codepoint <<= 8;
  codepoint |= ((unsigned char)ifs.get()) + 0x00;
  readSjisChar(codepoint, ofs, indentLevel);
}

void readSjisCmpChar(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
  if (dic == NULL) {
    throw TGenericException(T_SRCANDLINE,
                            "readSjisCmpChar()",
       std::string("Compressed character with NULL dictionary"));
  }
  
  int index = id - 0x80;
  
  CharacterDictionary::iterator findIt = dic->find(index);
  if (findIt == dic->end()) {
    throw TGenericException(T_SRCANDLINE,
                            "readSjisCmpChar()",
       std::string("Accessed non-existent dictionary entry: ")
             + TStringConversion::intToString(index,
                TStringConversion::baseHex));
  }
  
//  int code = findIt->second;
//  ofs.put((unsigned char)((code & 0xFF00) >> 8));
//  ofs.put((unsigned char)((code & 0x00FF)));
  
  int codepoint = findIt->second;
  readSjisChar(codepoint, ofs, indentLevel);
}

SmPceScriptOp smPceMsgFuncOps[] = {
  { 0x10, 0x10, "ifThenWhile", readIfThenWhile, writeIfThenWhile },
  { 0x11, 0x11, "return01", readSingleGeneric, writeSingleGeneric },
  { 0x12, 0x12, "return02", readSingleGeneric, writeSingleGeneric },
  { 0x13, 0x13, "dlgChoice", readFuncOp13, writeFuncOp13 },
  { 0x14, 0x14, "func_op14", readLocalsGeneric, writeLocalsGeneric },
  { 0x15, 0x15, "func_op15", readLocalsGeneric, writeLocalsGeneric },
  { 0x16, 0x16, "func_op16", readLocalsGeneric, writeLocalsGeneric },
  { 0x17, 0x17, "func_op17", readLocalsGeneric, writeLocalsGeneric },
  { 0x18, 0x18, "func_op18", readLocalsGeneric, writeLocalsGeneric },
  { 0x19, 0x19, "func_op19", readLocalsGeneric, writeLocalsGeneric },
  { 0x1A, 0x1A, "func_op1A", readLocalsGeneric, writeLocalsGeneric },
  { 0x1B, 0x1B, "func_op1B", readLocalsGeneric, writeLocalsGeneric },
  { 0x1C, 0x1C, "loadFile", readLocalsGeneric, writeLocalsGeneric },
  { 0x1D, 0x1D, "func_op1D", readLocalsGeneric, writeLocalsGeneric },
  { 0x1E, 0x1E, "loadAndRun", readLocalsGeneric, writeLocalsGeneric },
  { 0x1F, 0x1F, "loadAndCall", readLocalsGeneric, writeLocalsGeneric },
  { 0x20, 0x20, "func_op20", readLocalsGeneric, writeLocalsGeneric },
  { 0x21, 0x21, "func_op21", readLocalsGeneric, writeLocalsGeneric },
  { 0x22, 0x22, "playMusic", readLocalsGeneric, writeLocalsGeneric },
  { 0x23, 0x23, "playSound", readLocalsGeneric, writeLocalsGeneric },
  { 0x24, 0x24, "func_op24", readLocalsGeneric, writeLocalsGeneric },
  { 0x25, 0x25, "func_op25", readLocalsGeneric, writeLocalsGeneric },
  { 0x26, 0x26, "func_op26", readLocalsGeneric, writeLocalsGeneric }
};

void readMsgFunc(SmPceScriptOp* op, int id,
                  BlackT::TStream& ifs, std::ostream& ofs,
                  int indentLevel, CharacterDictionary* dic) {
//  printOpStart(ofs, op->name, indentLevel);
//  ofs << std::endl;
  
  int subid = ifs.readu8();
  SmPceScriptOp* subop = getScriptOp(
    smPceMsgFuncOps, sizeof(smPceMsgFuncOps) / sizeof(SmPceScriptOp), subid);
    
  if (subop == NULL) {
    throw TGenericException(T_SRCANDLINE,
                            "readMsgFunc()",
                            std::string("Unknown msgFunc subop: ")
                              + TStringConversion::intToString(subid,
                                    TStringConversion::baseHex));
  }
  
  subop->readFunc(subop, subid, ifs, ofs, indentLevel, dic);
  
  // check for playSound commands
  if (subid == 0x23) {
    msgNumToSoundFile[scriptMessages.size()] = lastScriptString;
    soundFileNames[lastScriptString] = false;
  }
  // check for loadFile commands
  else if (subid == 0x1C) {
    loadedFileNames[lastScriptString] = false;
  }
  // check for load? commands
  else if (subid == 0x1D) {
    loaded2FileNames[lastScriptString] = false;
  }
  
//  ofs << toHex(value);
  
}

SmPceScriptOp smPceMsgScriptOps[] = {
  { 0x00, 0x00, "ret", readSingleGeneric, writeSingleGeneric },
  { 0x01, 0x01, "callImmed", readCallImmed, writeCallImmed },
  { 0x02, 0x02, "ret02", readSingleGeneric, writeSingleGeneric },
  { 0x04, 0x04, "func", readMsgFunc, NULL },
  { 0x06, 0x06, "op06", readStringGeneric, writeStringGeneric },
  { 0x0A, 0x0A, "writeNybsLit", readWriteNybblesLiteral, writeWriteNybblesLiteral },
  { 0x0B, 0x0B, "writeNybs", readWriteNybbles, writeWriteNybbles },
  { 0x0C, 0x0C, "writeValueWord", readWriteValueWord, writeWriteValueWord },
  { 0x0D, 0x0D, "writeArrayWords", readWriteArrayWords, writeWriteArrayWords },
  { 0x0E, 0x0E, "writeArrayBytes", readWriteArrayWords, writeWriteArrayWords },
  { 0x0F, 0x0F, "if", readIf, writeIf },
  { 0x10, 0x10, "readLocals", readLocalsGeneric, writeLocalsGeneric },
  { 0x11, 0x11, "op11", readLocalsGeneric, writeLocalsGeneric },
  { 0x12, 0x12, "setFunc", readLocalsGeneric, writeLocalsGeneric },
  { 0x13, 0x13, "callFunc", readLocalsGeneric, writeLocalsGeneric },
  { 0x14, 0x14, "op14", readLocalsGeneric, writeLocalsGeneric },
  { 0x15, 0x15, "printNum", readLocalsGeneric, writeLocalsGeneric },
  { 0x16, 0x16, "op16", readLocalsGeneric, writeLocalsGeneric },
  { 0x17, 0x17, "op17", readLocalsGeneric, writeLocalsGeneric },
  { 0x18, 0x18, "op18", readLocalsGeneric, writeLocalsGeneric },
  { 0x19, 0x19, "op19", readLocalsGeneric, writeLocalsGeneric },
  { 0x1A, 0x1A, "op1A", readLocalsGeneric, writeLocalsGeneric },
  { 0x1B, 0x1B, "op1B_04_10", readVarThenMsgGeneric, writeVarThenMsgGeneric },
  { 0x1C, 0x1C, "op1C_04_11", readSingleGeneric, writeSingleGeneric },
  { 0x1D, 0x1D, "op1D_04_12", readSingleGeneric, writeSingleGeneric },
  { 0x1E, 0x1E, "op1E_04_13", readMsgGeneric, writeMsgGeneric },
  { 0x1F, 0x1F, "op1F_04_14", readLocalsGeneric, writeLocalsGeneric },
  { 0x60, 0x7F, "sjisLit", readSjisLiteral, NULL },
  { 0x80, 0xFF, "sjisCmp", readSjisCmpChar, NULL }
};

SmPceScriptOp* getVarScriptOp(int id) {
  return getScriptOp(smPceVarScriptOps,
    sizeof(smPceVarScriptOps) / sizeof(SmPceScriptOp), id);
}

SmPceScriptOp* getMsgScriptOp(int id) {
  return getScriptOp(smPceMsgScriptOps,
    sizeof(smPceMsgScriptOps) / sizeof(SmPceScriptOp), id);
}

SmPceScriptOp* getVarScriptOp(std::string name) {
  SmPceScriptOp* op = getScriptOp(smPceVarScriptOps,
    sizeof(smPceVarScriptOps) / sizeof(SmPceScriptOp), name);
  
  // hack to deal with my dumb handling of varMath
//  if (op == NULL) {
//    op = getScriptOp(smPceVarScriptVarMathOps,
//      sizeof(smPceVarScriptVarMathOps) / sizeof(SmPceScriptOp), name);
//  }
  
  return op;
}

SmPceScriptOp* getMsgScriptOp(std::string name) {
  SmPceScriptOp* op = getScriptOp(smPceMsgScriptOps,
    sizeof(smPceMsgScriptOps) / sizeof(SmPceScriptOp), name);
    
//  if (op == NULL) {
//    op = getScriptOp(smPceMsgFuncOps,
//      sizeof(smPceMsgFuncOps) / sizeof(SmPceScriptOp), name);
//  }
  
  return op;
}

SmPceScriptOp* getMsgScriptFuncOp(std::string name) {
  SmPceScriptOp* op = getScriptOp(smPceMsgFuncOps,
      sizeof(smPceMsgFuncOps) / sizeof(SmPceScriptOp), name);
  
  return op;
}


}
