#include "madou1pce/Madou1PceLineWrapper.h"
#include "util/TParse.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {

//const static int controlOpsStart = 0x40;
const static int controlOpsEnd   = 0x40;

const static int code_space   = 0x40;
const static int code_br      = 0x03;
const static int code_wait    = 0x05;
const static int code_clear_and_close = 0x06;
const static int code_clear   = 0x07;
const static int code_clear_goto_line2 = 0x08;
const static int code_end     = 0x0A;
const static int code_wait_for_voice = 0x2D;

Madou1PceLineWrapper::Madou1PceLineWrapper(BlackT::TStream& src__,
                ResultCollection& dst__,
                const BlackT::TThingyTable& thingy__,
                CharSizeTable sizeTable__,
                int xSize__,
                int ySize__)
  : TLineWrapper(src__, dst__, thingy__, xSize__, ySize__),
    sizeTable(sizeTable__),
    xBeforeWait(-1),
//    clearMode(clearMode_default),
    breakMode(breakMode_single) {
  
}

int Madou1PceLineWrapper::widthOfKey(int key) {
  if ((key == code_br)) return 0;
  // TODO: lots
  else if ((key < controlOpsEnd)) return 0;
  
//  std::cerr << std::hex << key << " " << std::dec << sizeTable[key] << " " << currentWordWidth << std::endl;
//  char c;
//  std::cin >> c;
  
  return sizeTable[key];
}

int Madou1PceLineWrapper::advanceWidthOfKey(int key) {
  return widthOfKey(key);
}

bool Madou1PceLineWrapper::isWordDivider(int key) {
  if (
      (key == code_br)
      || (key == code_space)
     ) return true;
  
  return false;
}

bool Madou1PceLineWrapper::isLinebreak(int key) {
  if (
      (key == code_br)
      ) return true;
  
  return false;
}

bool Madou1PceLineWrapper::isBoxClear(int key) {
  // END
  if ((key == code_end)
      || (key == code_clear)
      || (key == code_clear_goto_line2)) return true;
  
  return false;
}

void Madou1PceLineWrapper::onBoxFull() {
/*  if (clearMode == clearMode_default) {
    std::string content;
    if (lineHasContent) {
      // wait
      content += thingy.getEntry(code_wait);
      content += thingy.getEntry(code_br);
      currentScriptBuffer.write(content.c_str(), content.size());
    }
    // linebreak
    stripCurrentPreDividers();
    
    currentScriptBuffer.put('\n');
    xPos = 0;
    yPos = 0;
  }
  else if (clearMode == clearMode_messageSplit) {
    std::string content;
//      if (lineHasContent) {
      // wait
//        content += thingy.getEntry(code_wait);
//        content += thingy.getEntry(code_br);
      content += thingy.getEntry(code_end);
      content += "\n\n#ENDMSG()\n\n";
      currentScriptBuffer.write(content.c_str(), content.size());
//      }
    // linebreak
    stripCurrentPreDividers();
    
    xPos = 0;
    yPos = 0;
  } */
  
/*  std::string content;
  if (lineHasContent) {
    // wait
    content += thingy.getEntry(code_wait);
//    content += thingy.getEntry(code_br);
    content += linebreakString();
    currentScriptBuffer.write(content.c_str(), content.size());
  }
  // linebreak
  stripCurrentPreDividers();
  
  currentScriptBuffer.put('\n');
  xPos = 0;
  yPos = -1; */
  
  std::string content;
  if (lineHasContent) {
    // wait
    content += thingy.getEntry(code_wait);
//    content += thingy.getEntry(code_br);
//    content += linebreakString();
    content += thingy.getEntry(code_clear);
    currentScriptBuffer.write(content.c_str(), content.size());
  }
  // linebreak
  stripCurrentPreDividers();
  
  currentScriptBuffer.put('\n');
  currentScriptBuffer.put('\n');
  xPos = 0;
  yPos = -1;
}

//int Madou1PceLineWrapper::linebreakKey() {
//  return code_br;
//}

std::string Madou1PceLineWrapper::linebreakString() const {
  std::string breakString = thingy.getEntry(code_br);
  if (breakMode == breakMode_single) {
    return breakString;
  }
  else {
    return breakString + breakString;
  }
}

//int Madou1PceLineWrapper::linebreakHeight() const {
//  if (breakMode == breakMode_single) {
//    return 1;
//  }
//  else {
//    return 2;
//  }
//}

void Madou1PceLineWrapper::onSymbolAdded(BlackT::TStream& ifs, int key) {
/*  if (isLinebreak(key)) {
    if ((yPos != -1) && (yPos >= ySize - 1)) {
      flushActiveWord();
      
    }
  } */
}

void Madou1PceLineWrapper
    ::handleManualLinebreak(TLineWrapper::Symbol result, int key) {
  if ((key != code_br) || (breakMode == breakMode_single)) {
    TLineWrapper::handleManualLinebreak(result, key);
  }
  else {
    outputLinebreak(linebreakString());
  }
}

void Madou1PceLineWrapper::afterLinebreak(
    LinebreakSource clearSrc, int key) {
/*  if (clearSrc != linebreakBoxEnd) {
    if (spkrOn) {
      xPos = spkrLineInitialX;
    }
  } */
  
/*  if (clearSrc == linebreakManual) {
    if (breakMode == breakMode_double) {
      --yPos;
    }
  } */
}

void Madou1PceLineWrapper::beforeBoxClear(
    BoxClearSource clearSrc, int key) {
  if (((clearSrc == boxClearManual) && (key == code_wait))) {
    xBeforeWait = xPos;
  }
}

void Madou1PceLineWrapper::afterBoxClear(
  BoxClearSource clearSrc, int key) {
  // wait pauses but does not automatically break the line
  if (((clearSrc == boxClearManual) && (key == code_wait))) {
    xPos = xBeforeWait;
    yPos = -1;
/*    if (breakMode == breakMode_single) {
      yPos = -1;
    }
    else {
      yPos = -2;
    } */
  }
}

bool Madou1PceLineWrapper::processUserDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("SETBREAKMODE") == 0) {
    std::string type = TParse::matchName(ifs);
    
    if (type.compare("SINGLE") == 0) {
      breakMode = breakMode_single;
    }
    else if (type.compare("DOUBLE") == 0) {
      breakMode = breakMode_double;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "Madou1PceLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown break mode '"
                                + type
                                + "'");
    }
    
    return true;
  }
/*  else if (name.compare("PARABR") == 0) {
//    if (yPos >= ySize) {
//      onBoxFull();
//    }
//    else {
//      onBoxFull();
//    }
    flushActiveWord();
    outputLinebreak();
    return true;
  } */
//  else if (name.compare("ENDMSG") == 0) {
//    processEndMsg(ifs);
//    return true;
//  }
  
  return false;
}

}
