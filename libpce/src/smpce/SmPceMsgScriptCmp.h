#ifndef SMPCEMSGSCRIPTCMP_H
#define SMPCEMSGSCRIPTCMP_H


#include "util/TStream.h"
#include "smpce/SmPceScriptOps.h"
#include <vector>
#include <map>
#include <iostream>

namespace Pce {


class SmPceMsgScriptCmp {
public:
  SmPceMsgScriptCmp(std::istream& ifs__,
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
