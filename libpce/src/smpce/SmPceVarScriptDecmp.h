#ifndef SMPCEVARSCRIPTDECMP_H
#define SMPCEVARSCRIPTDECMP_H


#include "util/TStream.h"
#include "smpce/SmPceScriptOps.h"
#include <iostream>

namespace Pce {


class SmPceVarScriptDecmp {
public:
  SmPceVarScriptDecmp(BlackT::TStream& ifs__,
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
