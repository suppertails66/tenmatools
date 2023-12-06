#include "smpce/SmPceVarScriptDecmp.h"
#include "smpce/SmPceScriptOps.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


SmPceVarScriptDecmp::SmPceVarScriptDecmp(BlackT::TStream& ifs__,
                                         std::ostream& ofs__,
                                         int indentLevel__,
                                         CharacterDictionary* dic__)
  : ifs(&ifs__),
    ofs(&ofs__),
    indentLevel(indentLevel__),
    dic(dic__) { }

void SmPceVarScriptDecmp::operator()() {
//  int test = smPceVarScriptOps[0].id;
  
  opIndent(*ofs, indentLevel);
  *ofs << "[" << std::endl;
  
  int id;
  do {
    
    if (ifs->remaining() <= 0) {
      throw TGenericException(T_SRCANDLINE,
                              "SmPceVarScriptDecmp::operator()()",
                              std::string("Unterminated varscript"));
    }
    
    id = ifs->readu8();
    SmPceScriptOp* op = getVarScriptOp(id);
    if (op == NULL) {
      throw TGenericException(T_SRCANDLINE,
                              "SmPceVarScriptDecmp::operator()()",
                              std::string("Unknown varscript op: ")
                                + TStringConversion::intToString(id,
                                    TStringConversion::baseHex));
    }
    
//    std::cerr << "startvar: " << op->name << " " << std::hex << ifs->tell() << std::endl;
    op->readFunc(op, id, *ifs, *ofs, indentLevel + 1, dic);
//    std::cerr << "endvar: " << std::hex << ifs->tell() << std::endl;
    
    
  } while (id != 0x03);
  
  opIndent(*ofs, indentLevel);
  *ofs << "]" << std::endl;
}


}
