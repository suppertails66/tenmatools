#ifndef SMPCEVARSCRIPTCMP_H
#define SMPCEVARSCRIPTCMP_H


#include "util/TStream.h"
#include "smpce/SmPceScriptOps.h"
#include <vector>
#include <map>
#include <iostream>

namespace Pce {


class SmPceVarScriptCmp {
public:
  SmPceVarScriptCmp(std::istream& ifs__,
                      BlackT::TStream& ofs__,
                      int& lineNum__,
                      MessageDictionary* dic__ = NULL);
  
  void operator()();
  
protected:
  
  std::istream* ifs;
  BlackT::TStream* ofs;
  int* lineNum;
  MessageDictionary* dic;
  
};


}


#endif
