#ifndef SMPCEMSGSCRIPTDECMP_H
#define SMPCEMSGSCRIPTDECMP_H


#include "util/TStream.h"
#include "smpce/SmPceScriptOps.h"
#include <vector>
#include <map>
#include <iostream>

namespace Pce {


class SmPceMsgScriptDecmp {
public:
  SmPceMsgScriptDecmp(BlackT::TStream& ifs__,
                      std::ostream& ofs__,
                      int indentLevel__ = 0,
                      CharacterDictionary* dic__ = NULL);
  
  void operator()();
  
protected:
  
  BlackT::TStream* ifs;
  std::ostream* ofs;
  int indentLevel;
  CharacterDictionary* dic;
  
};


}


#endif
