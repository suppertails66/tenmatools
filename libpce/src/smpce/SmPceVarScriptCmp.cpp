#include "smpce/SmPceVarScriptCmp.h"
#include "smpce/SmPceScriptOps.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <cstring>
#include <iostream>

using namespace BlackT;

namespace Pce {


SmPceVarScriptCmp::SmPceVarScriptCmp(std::istream& ifs__,
                                         BlackT::TStream& ofs__,
                                         int& lineNum__,
                                         MessageDictionary* dic__)
  : ifs(&ifs__),
    ofs(&ofs__),
    lineNum(&lineNum__),
    dic(dic__) { }

void SmPceVarScriptCmp::operator()() {

//  opIndent(*ofs, indentLevel);
//  *ofs << "{" << std::endl;
  matchSpacedSymbol(*ifs, "[", *lineNum);
  
//  int id;
  do {
//    if (ifs->remaining() <= 0) {
    if (!ifs->good()) {
//      throw TGenericException(T_SRCANDLINE,
//                              "SmPceVarScriptCmp::operator()()",
//                              std::string("Unterminated msgscript"));
      break;
    }
    
    skipSpace(*ifs, *lineNum);
    if (ifs->peek() == ']') break;
    
//    id = ifs->readu8();
    std::string opname = getSpacedSymbol(*ifs, *lineNum);
    
//    // check for end of message
//    if ((id < 0x60) && (!activeMessage.empty())) {
//      scriptMessages.push_back(activeMessage);
//      activeMessage.clear();
//      *ofs << std::endl;
//    }
    // check for start of message
//    if ((opname.size() >= 4)
//        && (opname.substr(0, 4).compare("<MSG") == 0)) {
//      // ...
//      continue;
//    }
    
//    int subidPos = ifs->tell();
//    SmPceScriptOp* op = getMsgScriptOp(id);
//    if (op == NULL) {
//      throw TGenericException(T_SRCANDLINE,
//                              "SmPceVarScriptCmp::operator()()",
//                              std::string("Unknown msgscript op: ")
//                                + TStringConversion::intToString(id,
//                                    TStringConversion::baseHex));
//    }
    
    SmPceScriptOp* op = getVarScriptOp(opname);
    if (op == NULL) {
      throw TGenericException(T_SRCANDLINE,
                              "SmPceVarScriptCmp::operator()()",
                              std::string("Line ")
                                + TStringConversion::intToString(*lineNum)
                                + ":\n  "
                                + "Unknown varscript op: "
                                + opname);
    }
    
//    op->readFunc(op, id, *ifs, *ofs, indentLevel + 1, dic);

//    std::cerr << "start: " << op->name << " " << std::hex << ifs->tell() << std::endl;
//    std::cerr << "var " << op->name << " " << *lineNum << std::endl;
    op->writeFunc(op, *ifs, *ofs, *lineNum, dic);
//    std::cerr << "end: " << std::hex << ifs->tell() << std::endl;
    
    // check for terminators
//    if (id == 0x04) {
//      int newPos = ifs->tell();
//      ifs->seek(subidPos);
//      int subid = ifs->readu8();
//      ifs->seek(newPos);
//      
//      // return01/return02
//      if ((subid == 0x11) || (subid == 0x12)) {
//        break;
//      }
//    }
    
/*    if ((std::strcmp(op->name, "return01") == 0)
        || (std::strcmp(op->name, "return02") == 0)) {
      break;
    }
    else if ((op->idLow == 0x00)
        || (op->idLow == 0x02)) {
      break;
    } */
    
//  } while (!((id == 0x00) || (id == 0x02)));
  } while (true);
  
//  opIndent(*ofs, indentLevel);
//  *ofs << "}" << std::endl;
  matchSpacedSymbol(*ifs, "]", *lineNum);
}


}
