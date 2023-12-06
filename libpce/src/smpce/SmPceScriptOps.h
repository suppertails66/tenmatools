#ifndef SMPCESCRIPTOPS_H
#define SMPCESCRIPTOPS_H


#include "util/TStream.h"
#include <map>
#include <iostream>

#include <vector>
#include <string>

namespace Pce {


typedef std::map<int, int> CharacterDictionary;

typedef std::map<std::string, std::string> MessageDictionary;

struct SmPceScriptOp {
  int idLow;
  int idHigh;
  const char* name;
  void (*readFunc)(SmPceScriptOp* op,
                   int id,
                   BlackT::TStream& ifs,
                   std::ostream& ofs,
                   int indentLevel,
                   CharacterDictionary* dic);
  void (*writeFunc)(SmPceScriptOp* op,
                    std::istream& ifs, BlackT::TStream& ofs,
                    int& lineNum, MessageDictionary* dic);
};

std::string prepReadString(const std::string& input,
                           std::string& label,
                           std::string& output);
//std::string prepWriteString(const std::string& input);

void skipSpace(std::istream& ifs, int& lineNum);
std::string getSpacedSymbol(std::istream& ifs, int& lineNum);
int getInt(std::istream& ifs, int& lineNum);
bool checkSpacedSymbol(std::istream& ifs, std::string sym, int& lineNum);
void matchSpacedSymbol(std::istream& ifs, std::string sym, int& lineNum);
bool checkChar(std::istream& ifs, char sym, int& lineNum);
void matchChar(std::istream& ifs, char sym, int& lineNum);

//extern SmPceScriptOp smPceVarScriptOps[];
//extern SmPceScriptOp smPceMsgScriptOps[];
extern std::map<std::string, bool> scriptStrings;
extern std::string lastScriptString;

extern std::map<std::string, bool> loadedFileNames;
extern std::map<std::string, bool> loaded2FileNames;

extern std::map<int, std::string> msgNumToSoundFile;
extern std::map<std::string, bool> soundFileNames;

extern std::vector<std::string> scriptMessages;
extern std::string activeMessage;

void opIndent(std::ostream& ofs, int indentLevel);

SmPceScriptOp* getVarScriptOp(int id);
SmPceScriptOp* getMsgScriptOp(int id);
SmPceScriptOp* getVarScriptOp(std::string name);
SmPceScriptOp* getMsgScriptOp(std::string name);
SmPceScriptOp* getMsgScriptFuncOp(std::string name);

/*class SmPceVarScriptOps {
public:
  
  
  
protected:
  
};*/


}


#endif
