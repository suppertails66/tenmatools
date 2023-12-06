#include "smpce/SmPceMsgScriptDecmp.h"
#include "smpce/SmPceScriptOps.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


SmPceMsgScriptDecmp::SmPceMsgScriptDecmp(BlackT::TStream& ifs__,
                                         std::ostream& ofs__,
                                         int indentLevel__,
                                         CharacterDictionary* dic__)
  : ifs(&ifs__),
    ofs(&ofs__),
    indentLevel(indentLevel__),
    dic(dic__) { }

void SmPceMsgScriptDecmp::operator()() {
//  int test = smPceVarScriptOps[0].id;
  
  opIndent(*ofs, indentLevel);
  *ofs << "{" << std::endl;
  
  int id;
  do {
    if (ifs->remaining() <= 0) {
//      throw TGenericException(T_SRCANDLINE,
//                              "SmPceMsgScriptDecmp::operator()()",
//                              std::string("Unterminated msgscript"));
      break;
    }
    
    id = ifs->readu8();
    
    // check for end of message
    if ((id < 0x60) && (!activeMessage.empty())) {
      scriptMessages.push_back(activeMessage);
      activeMessage.clear();
      *ofs << std::endl;
    }
    
    int subidPos = ifs->tell();
    SmPceScriptOp* op = getMsgScriptOp(id);
    if (op == NULL) {
      throw TGenericException(T_SRCANDLINE,
                              "SmPceMsgScriptDecmp::operator()()",
                              std::string("Unknown msgscript op: ")
                                + TStringConversion::intToString(id,
                                    TStringConversion::baseHex));
    }
    
//    std::cerr << "start: " << op->name << " " << std::hex << ifs->tell() << std::endl;
    op->readFunc(op, id, *ifs, *ofs, indentLevel + 1, dic);
//    std::cerr << "end: " << std::hex << ifs->tell() << std::endl;
    
    // check for terminators
    if (id == 0x04) {
      int newPos = ifs->tell();
      ifs->seek(subidPos);
      int subid = ifs->readu8();
      ifs->seek(newPos);
      
      // return01/return02
      if ((subid == 0x11) || (subid == 0x12)) {
//        break;
      }
    }
    // check for sound files
//    else if (id == 0x23) {
//      msgNumToSoundFile[scriptMessages.size()] = lastScriptString;
//    }
    
  } while (!((id == 0x00) || (id == 0x02)));
  
  opIndent(*ofs, indentLevel);
  *ofs << "}" << std::endl;
}


}
