#include "yuna/YunaLineWrapper.h"
#include "util/TParse.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {

//const static int controlOpsStart = 0x40;
const static int controlOpsEnd   = 0x50;

const static int code_end     = 0x00;
const static int code_space   = 0x50;
const static int code_br      = 0x0A;
/*const static int code_wait    = 0x05;
const static int code_clear_and_close = 0x06;
const static int code_clear   = 0x07;
const static int code_clear_goto_line2 = 0x08;
const static int code_wait_for_voice = 0x2D; */

YunaLineWrapper::YunaLineWrapper(BlackT::TStream& src__,
                ResultCollection& dst__,
                const BlackT::TThingyTable& thingy__,
                CharSizeTable sizeTable__,
                int xSize__,
                int ySize__)
  : TLineWrapper(src__, dst__, thingy__, xSize__, ySize__),
    sizeTable(sizeTable__),
    xBeforeWait(-1),
//    clearMode(clearMode_default),
    breakMode(breakMode_single),
    failOnBoxOverflow(false) {
  
}

int YunaLineWrapper::getLocalBrId() const {
  // HACK
  // (br char differs between regular and cutscene fonts)
  return thingy.matchTableEntry("[br]").id;
}

int YunaLineWrapper::widthOfKey(int key) {
  if ((key == getLocalBrId())) return 0;
  else if ((key < controlOpsEnd)) return 0;
  
//  std::cerr << std::hex << key << " " << std::dec << sizeTable[key] << " " << currentWordWidth << std::endl;
//  char c;
//  std::cin >> c;
  
  return sizeTable[key];
}

int YunaLineWrapper::advanceWidthOfKey(int key) {
  return widthOfKey(key);
}

bool YunaLineWrapper::isWordDivider(int key) {
  if (
      (key == getLocalBrId())
      || (key == code_space)
     ) return true;
  
  return false;
}

bool YunaLineWrapper::isLinebreak(int key) {
  if (
      (key == getLocalBrId())
      ) return true;
  
  return false;
}

bool YunaLineWrapper::isBoxClear(int key) {
  // TODO
  if ((key == code_end)
//      || (key == code_clear)
//      || (key == code_clear_goto_line2))
    ) return true;
  
  return false;
}

void YunaLineWrapper::onBoxFull() {
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
    // TODO?
    // wait
//    content += thingy.getEntry(code_wait);
    // full box + linebreak = automatic box wait + clear
    content += thingy.getEntry(getLocalBrId());
    
//    content += thingy.getEntry(code_br);
//    content += linebreakString();

//    content += thingy.getEntry(code_clear);
    
    currentScriptBuffer.write(content.c_str(), content.size());
  }
  // linebreak
  stripCurrentPreDividers();
  
  currentScriptBuffer.put('\n');
  currentScriptBuffer.put('\n');
  xPos = 0;
  yPos = -1;
}

//int YunaLineWrapper::linebreakKey() {
//  return code_br;
//}

std::string YunaLineWrapper::linebreakString() const {
  std::string breakString = thingy.getEntry(getLocalBrId());
  if (breakMode == breakMode_single) {
    return breakString;
  }
  else {
    return breakString + breakString;
  }
}

//int YunaLineWrapper::linebreakHeight() const {
//  if (breakMode == breakMode_single) {
//    return 1;
//  }
//  else {
//    return 2;
//  }
//}

void YunaLineWrapper::onSymbolAdded(BlackT::TStream& ifs, int key) {
/*  if (isLinebreak(key)) {
    if ((yPos != -1) && (yPos >= ySize - 1)) {
      flushActiveWord();
      
    }
  } */
}

void YunaLineWrapper
    ::handleManualLinebreak(TLineWrapper::Symbol result, int key) {
  if ((key != getLocalBrId()) || (breakMode == breakMode_single)) {
    TLineWrapper::handleManualLinebreak(result, key);
  }
  else {
    outputLinebreak(linebreakString());
  }
}

void YunaLineWrapper::afterLinebreak(
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

void YunaLineWrapper::beforeBoxClear(
    BoxClearSource clearSrc, int key) {
//  if (((clearSrc == boxClearManual) && (key == code_wait))) {
//    xBeforeWait = xPos;
//  }
  if (failOnBoxOverflow && (clearSrc != boxClearManual)) {
    throw TGenericException(T_SRCANDLINE,
                            "YunaLineWrapper::beforeBoxClear()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ": box overflow");
  }
}

void YunaLineWrapper::afterBoxClear(
  BoxClearSource clearSrc, int key) {
  // wait pauses but does not automatically break the line
//  if (((clearSrc == boxClearManual) && (key == code_wait))) {
//    xPos = xBeforeWait;
//    yPos = -1;
/*    if (breakMode == breakMode_single) {
      yPos = -1;
    }
    else {
      yPos = -2;
    } */
//  }
}

bool YunaLineWrapper::processUserDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("STARTSTRING") == 0) {
    // don't care
    TParse::matchInt(ifs);
    // don't care
    TParse::matchChar(ifs, ',');
    TParse::matchInt(ifs);
    // don't care
    TParse::matchChar(ifs, ',');
    TParse::matchInt(ifs);
    // isLiteral
    TParse::matchChar(ifs, ',');
    
    // turn on literal mode if literal
    copyEverythingLiterally = (TParse::matchInt(ifs) != 0);
    
    return false;
  }
  else if (name.compare("ENDSTRING") == 0) {
    // turn off literal mode
    copyEverythingLiterally = false;
    
    // HACK: oops this is dumb
      // output current word
      flushActiveWord();
      
      beforeBoxClear(boxClearManual, code_end);
      
//      std::string content = result.getContent(thingy);
//      currentScriptBuffer.write(content.c_str(), content.size());
//      currentScriptBuffer.put('\n');
//      currentScriptBuffer.put('\n');
        
      xPos = 0;
      yPos = 0;
      pendingAdvanceWidth = 0;
      
      afterBoxClear(boxClearManual, code_end);
    
    return false;
  }
  else if (name.compare("SETBREAKMODE") == 0) {
    std::string type = TParse::matchName(ifs);
    
    if (type.compare("SINGLE") == 0) {
      breakMode = breakMode_single;
    }
    else if (type.compare("DOUBLE") == 0) {
      breakMode = breakMode_double;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "YunaLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown break mode '"
                                + type
                                + "'");
    }
    
    return true;
  }
  else if (name.compare("SETFAILONBOXOVERFLOW") == 0) {
    int value = TParse::matchInt(ifs);
    failOnBoxOverflow = (value != 0);
    
    return true;
  }
  else if (name.compare("BREAKBOX") == 0) {
/*    flushActiveWord();
    int count = ySize - yPos;
    for (int i = 0; i < count; i++) {
      outputLinebreak();
    } */
    
    if (ySize == -1) {
      throw TGenericException(T_SRCANDLINE,
                              "YunaLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": tried to break infinitely-high box");
    }
    
    flushActiveWord();
    std::string breakstr = thingy.getEntry(getLocalBrId());
    while (yPos < (ySize - 1)) {
      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
      ++yPos;
    }
    
    onBoxFull();
    
    return true;
  }
  else if (name.compare("BREAKGROUP") == 0) {
    if (yPos > ((ySize/2) - 1)) {
      throw TGenericException(T_SRCANDLINE,
                              "YunaLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": tried to break overflowed group");
    }
    
//    if (yPos != ((ySize/2) - 1)) {
      flushActiveWord();
      std::string breakstr = thingy.getEntry(getLocalBrId());
      while (yPos < ((ySize/2) - 1)) {
        currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
        ++yPos;
      }
    
      onBoxFull();
//    }
    
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
