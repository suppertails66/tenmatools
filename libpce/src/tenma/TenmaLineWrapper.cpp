#include "tenma/TenmaLineWrapper.h"
#include "tenma/TenmaMsgConsts.h"
#include "util/TParse.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {

//const static int controlOpsStart = 0x40;

const static int code_br4       = 0xEB;
const static int code_forceBoxEnd = 0xEC;
const static int code_forceBoxPause = 0xED;
const static int code_clear    = 0xF2;
const static int code_br       = 0xFE;
const static int code_end      = 0xFF;

// TODO
//const static int code_clear    = 0x03;
//const static int code_end16    = 0x16;
//const static int code_end1A    = 0x1A;
const static int fontBase = 0x08;
const static int code_period   = 0x3F + fontBase;
const static int code_exclaim  = 0x41 + fontBase;
const static int code_question = 0x42 + fontBase;
const static int code_rquotesing = 0x43 + fontBase;
const static int code_rquote   = 0x49 + fontBase;
const static int code_hyphen   = 0x4A + fontBase;
const static int code_ellipsis = 0x4B + fontBase;
const static int code_dash     = 0x4D + fontBase;
const static int code_space1px = 0x4F + fontBase;
const static int code_space8px = 0x58 + fontBase;
const static int code_space4px = 0x5D + fontBase;

// TODO
const static int code_emphModeToggle = 0x5F + fontBase;

TenmaLineWrapper::TenmaLineWrapper(BlackT::TStream& src__,
                ResultCollection& dst__,
                const BlackT::TThingyTable& thingy__,
                CharSizeTable sizeTable__,
                CharSizeTable sizeTableEmph__,
                bool isNonstd__,
                int xSize__,
                int ySize__,
                int controlOpsStart__)
  : TLineWrapper(src__, dst__, thingy__, xSize__, ySize__),
    controlOpsStart(default_controlOpsStart),
    sizeTable(sizeTable__),
    sizeTableEmph(sizeTableEmph__),
    isNonstd(isNonstd__),
    xBeforeWait(-1),
//    clearMode(clearMode_default),
    breakMode(breakMode_single),
    failOnBoxOverflow(false),
    doubleWidthModeOn(false),
    doubleWidthCharsInWord(false),
    padAndCenter_on(false),
    padAndCenter_leftPad(0),
    padAndCenter_width(0),
    longestLineLen(0),
    emphModeOn(false),
    br4ModeOn(false) {
  
}

int TenmaLineWrapper::getLocalBrId() const {
  // HACK
  // (br char differs between regular and cutscene fonts)
  if (br4ModeOn)
    return thingy.matchTableEntry("[br4]").id;
  else
    return thingy.matchTableEntry("\\n").id;
}

int TenmaLineWrapper::getLocalSpaceId() const {
  // HACK
  return thingy.matchTableEntry(" ").id;
}

/*int TenmaLineWrapper::getLocalEspId() const {
  // HACK
  return thingy.matchTableEntry("[esp]").id;
}*/

int TenmaLineWrapper::widthOfKey(int key) {
  if ((key == getLocalBrId())) return 0;
  else if ((key >= controlOpsStart)) return 0;
  
//  if (key == code_emphModeToggle) return 0;
  
//  std::cerr << std::hex << key << " " << std::dec << sizeTable[key] << " " << currentWordWidth << " " << xPos << std::endl;
//  char c;
//  std::cin >> c;
  
  int result = sizeTable[key - fontBase];
  
  if (emphModeOn
      && (key != getLocalSpaceId()))
    result = sizeTableEmph[key - fontBase];
  
  if (doubleWidthModeOn) result *= 2;
  return result;
}

int TenmaLineWrapper::advanceWidthOfKey(int key) {
  return widthOfKey(key);
}

bool TenmaLineWrapper::isWordDivider(int key) {
  if ((key == getLocalBrId())
      || (key == getLocalSpaceId()
      // TRANSLATION ONLY
//      || (key == TenmaTextScript::op_13)
      )
     ) return true;
  
  return false;
}

bool TenmaLineWrapper::isVisibleWordDivider(int key, int nextKey) {
  if (isNonstd) return false;
  
  if ((key == code_hyphen)
      || (key == code_ellipsis)
      || (key == code_dash)) {
    // if followed by punctuation, do not treat as divider
    if ((nextKey == code_exclaim)
        || (nextKey == code_question)
        || (nextKey == code_rquote)
        || (nextKey == code_rquotesing)) {
      return false;
    }
    
    return true;
  }
  
  return false;
}

bool TenmaLineWrapper::isLinebreak(int key) {
  if ((key == getLocalBrId()
      || (key == code_br4))
      // TRANSLATION ONLY
//      || (key == TenmaTextScript::op_13)
      ) return true;
  
  return false;
}

bool TenmaLineWrapper::isBoxClear(int key) {
  // TODO
  if (
//      (key == code_end16)
//      || (key == code_end1A)
//      || 
      (key == code_clear)
    ) return true;
  
  return false;
}

void TenmaLineWrapper::onBoxFull() {
  // TODO: pretty sure the game has autoscrolling
  // (which isn't normally used)
/*  std::string content;
  if (lineHasContent) {
    // wait
    content += thingy.getEntry(code_wait);
    // full box + linebreak = automatic box wait + clear
//    content += thingy.getEntry(getLocalBrId());
    
//    content += thingy.getEntry(code_br);
//    content += linebreakString();

//    content += thingy.getEntry(code_clear);
    
    currentScriptBuffer.write(content.c_str(), content.size());
  }
  // linebreak
  stripCurrentPreDividers(); */
  
  // TODO?
  std::string content;
//  if (lineHasContent) {
    // full box + linebreak = automatic box wait
    content += thingy.getEntry(getLocalBrId());
    
    currentScriptBuffer.write(content.c_str(), content.size());
//  }
  // linebreak
  stripCurrentPreDividers();
  
  currentScriptBuffer.put('\n');
  currentScriptBuffer.put('\n');
  
  currentScriptBuffer.writeString("// AUTOMATICALLY GENERATED BOX BREAK");
  
  currentScriptBuffer.put('\n');
  currentScriptBuffer.put('\n');
  xPos = 0;
//  yPos = -1;
  // ???
  yPos = 0;
}

//int TenmaLineWrapper::linebreakKey() {
//  return code_br;
//}

std::string TenmaLineWrapper::linebreakString() const {
  std::string breakString = thingy.getEntry(getLocalBrId());
  if (breakMode == breakMode_single) {
    return breakString;
  }
  else {
    return breakString + breakString;
  }
}

//int TenmaLineWrapper::linebreakHeight() const {
//  if (breakMode == breakMode_single) {
//    return 1;
//  }
//  else {
//    return 2;
//  }
//}

void TenmaLineWrapper::onSymbolAdded(BlackT::TStream& ifs, int key) {
/*  if (isLinebreak(key)) {
    if ((yPos != -1) && (yPos >= ySize - 1)) {
      flushActiveWord();
      
    }
  } */
  
/*  if (key == TenmaTextScript::op_size1x) {
    doubleWidthModeOn = false;
  }
  else if ((key == TenmaTextScript::op_size2x)
          || (key == TenmaTextScript::op_size3x)) {
    doubleWidthModeOn = true;
  }
  else if (!TenmaTextScript::isOp(key)) {
    if (doubleWidthModeOn) doubleWidthCharsInWord = true;
  }
  else */
  if (key == code_emphModeToggle) {
    emphModeOn = !emphModeOn;
  }
}

void TenmaLineWrapper
    ::handleManualLinebreak(TLineWrapper::Symbol result, int key) {
  if ((key != getLocalBrId()) || (breakMode == breakMode_single)) {
    TLineWrapper::handleManualLinebreak(result, key);
  }
  else {
    outputLinebreak(linebreakString());
  }
}

void TenmaLineWrapper::onLineContentStarted() {
  lineContentStartOffsets.push_back(currentScriptBuffer.tell());
}

void TenmaLineWrapper::beforeLinebreak(
    LinebreakSource clearSrc, int key) {
  if (clearSrc == linebreakManual) {
    if ((key != code_br4)
        || br4ModeOn) {
      applyPadAndCenterToCurrentLine();
      doLineEndPadChecks();
    }
    
    if (key == code_br4)
      br4ModeOn = true;
  }
}

void TenmaLineWrapper::afterLinebreak(
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
  
  // the double-width text modes can't have text automatically broken
  // because there aren't enough lines in the box for it,
  // so it's an immediate error
  if (
//      doubleWidthModeOn
      doubleWidthCharsInWord
//    && (clearSrc != linebreakManual)
      ) {
    throw TGenericException(T_SRCANDLINE,
                            "TenmaLineWrapper::afterLinebreak()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ": double-width mode box overflow");
  }
}

void TenmaLineWrapper::beforeBoxClear(
    BoxClearSource clearSrc, int key) {
//  if (((clearSrc == boxClearManual) && (key == code_wait))) {
//    xBeforeWait = xPos;
//  }
  if (failOnBoxOverflow && (clearSrc != boxClearManual)) {
    throw TGenericException(T_SRCANDLINE,
                            "TenmaLineWrapper::beforeBoxClear()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ": box overflow");
  }
  
  doLineEndPadChecks();
  
//  currentScriptBuffer.put('\n');
//  currentScriptBuffer.put('\n');
}

void TenmaLineWrapper::afterBoxClear(
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

void TenmaLineWrapper::beforeWordFlushed() {
  doubleWidthCharsInWord = false;
}

char TenmaLineWrapper::literalOpenSymbol() const {
  return myLiteralOpenSymbol;
}

char TenmaLineWrapper::literalCloseSymbol() const {
  return myLiteralCloseSymbol;
}

bool TenmaLineWrapper::processUserDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("STARTSTRING") == 0) {
/*    // don't care
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
    copyEverythingLiterally = (TParse::matchInt(ifs) != 0); */
    
    return false;
  }
  else if (name.compare("ENDSTRING") == 0) {
    // HACK: for pad+center.
    // i don't remember how much of this is actually necessary,
    // this is pure copy and paste from another file
    // FIXME
      // output current word
      flushActiveWord();
      
      doLineEndPadChecks();
      applyPadAndCenterToCurrentLine();
//      applyPadAndCenterToCurrentBlock();
//      padAndCenter_on = false;
      
      beforeBoxClear(boxClearManual, code_end);
        
      xPos = 0;
      yPos = 0;
      pendingAdvanceWidth = 0;
      
      lineContentStartOffsets.clear();
      longestLineLen = 0;
      
      afterBoxClear(boxClearManual, code_end);
      
      lineHasContent = false;
      currentLineContentStartInBuffer = -1;
    
    br4ModeOn = false;
    processEndMsg(ifs);
    return true;
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
                              "TenmaLineWrapper::processUserDirective()",
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
  else if (name.compare("SETPADANDCENTER") == 0) {
    padAndCenter_leftPad = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    padAndCenter_width = TParse::matchInt(ifs);
    
    padAndCenter_on = true;
    
    return true;
  }
  else if (name.compare("DISABLEPADANDCENTER") == 0) {
    // output current word
    flushActiveWord();
    applyPadAndCenterToCurrentLine();
    padAndCenter_on = false;
    
    return true;
  }
  else if ((name.compare("BREAKBOX") == 0)) {
/*    flushActiveWord();
    int count = ySize - yPos;
    for (int i = 0; i < count; i++) {
      outputLinebreak();
    } */
    
    int targetSize = ySize - 1;
    
    if (ySize == -1) {
      throw TGenericException(T_SRCANDLINE,
                              "TenmaLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": tried to break infinitely-high box");
    }
    
    flushActiveWord();
    std::string breakstr = thingy.getEntry(getLocalBrId());
    while (yPos < targetSize) {
      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
      ++yPos;
    }
    
    onBoxFull();
    
    return true;
  }
  else if ((name.compare("BOXPAUSE") == 0)
           || (name.compare("BOXEND") == 0)) {
    if (ySize == -1) {
/*      throw TGenericException(T_SRCANDLINE,
                              "TenmaLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": tried to break infinitely-high box");*/
      // for autowrap, where we don't know how many lines are needed
      // to reach the end of the box
/*      if (name.compare("BOXPAUSE") == 0) {
        flushActiveWord();
        std::string str = thingy.getEntry(code_forceBoxPause);
        currentScriptBuffer.write(str.c_str(), str.size());
        stripCurrentPreDividers();
        currentScriptBuffer.put('\n');
        currentScriptBuffer.put('\n');
        xPos = 0;
        yPos = 0;
      }*/
      flushActiveWord();
      applyPadAndCenterToCurrentLine();
      std::string str;
      if (name.compare("BOXPAUSE") == 0) {
        str = thingy.getEntry(code_forceBoxPause);
      }
      else {
        str = thingy.getEntry(code_forceBoxEnd);
      }
      currentScriptBuffer.write(str.c_str(), str.size());
      stripCurrentPreDividers();
      currentScriptBuffer.put('\n');
      currentScriptBuffer.put('\n');
      xPos = 0;
      yPos = 0;
      lineHasContent = false;
      br4ModeOn = false;
      return true;
    }
    
//    std::cerr << "initial: " << yPos << std::endl;
    
    int targetSize = ySize - 1;
    if (name.compare("BOXPAUSE") == 0)
      targetSize = ySize - 0;
    
    flushActiveWord();
    applyPadAndCenterToCurrentLine();
    std::string breakstr = thingy.getEntry(getLocalBrId());
    while (yPos < targetSize) {
      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
      ++yPos;
    }
    
//    std::cerr << "semifinal: " << yPos << std::endl;
    
    if (name.compare("BOXPAUSE") == 0) {
//      onBoxFull();
      stripCurrentPreDividers();
      
      currentScriptBuffer.put('\n');
      currentScriptBuffer.put('\n');
      xPos = 0;
      yPos = 0;
    }
    
    lineHasContent = false;
//    std::cerr << "final: " << yPos << std::endl;
    return true;
  }
  else if (name.compare("BREAKGROUP") == 0) {
    if (yPos > ((ySize/2) - 1)) {
      throw TGenericException(T_SRCANDLINE,
                              "TenmaLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": tried to break overflowed group");
    }
      
    std::string breakstr = thingy.getEntry(getLocalBrId());
  
//    if (currentWordBuffer.size() <= 0) {
//      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
//    }
    
//    if (yPos != ((ySize/2) - 1)) {
//      std::cerr << "1: " << ySize << " " << yPos << std::endl;
    flushActiveWord();
//      std::cerr << "2: " << ySize << " " << yPos << std::endl;
    while (yPos < ((ySize/2) - 1)) {
//      while (yPos < ((ySize/2))) {
      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
      ++yPos;
    }
//      std::cerr << "3: " << ySize << " " << yPos << std::endl;
  
    // HACK to correctly handle empty groups
    lineHasContent = true;
    
    onBoxFull();
//      std::cerr << "4: " << ySize << " " << yPos << std::endl;
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

std::string TenmaLineWrapper::getPadString(int width) {
  std::string output;
  if (width <= 0) return output;
  
  while (width > 0) {
    if (width >= 8) {
      output += thingy.getEntry(code_space8px);
      width -= 8;
    }
    else if (width >= 4) {
      output += thingy.getEntry(code_space4px);
      width -= 4;
    }
    else {
      output += thingy.getEntry(code_space1px);
      width -= 1;
    }
  }
  
/*  output += thingy.getEntry(TenmaTextScript::op_spaces);
  output += "<$";
  std::string valstr
    = TStringConversion::intToString(width, TStringConversion::baseHex)
        .substr(2, std::string::npos);
  while (valstr.size() < 2) valstr = std::string("0") + valstr;
  output += valstr;
  output += ">";*/
  return output;
}

void TenmaLineWrapper::doLineEndPadChecks() {
  if (xPos > longestLineLen) longestLineLen = xPos;
}

void TenmaLineWrapper::applyPadAndCenterToCurrentLine() {
  if (!padAndCenter_on) return;
  if (!lineHasContent) return;

  TBufStream temp;
  
  // copy script buffer up to start of current line content
  currentScriptBuffer.seek(0);
  temp.writeFrom(currentScriptBuffer, currentLineContentStartInBuffer);
  
  // insert padding at start of current line
//  std::cerr << "x: " << xPos << std::endl;
//  std::cerr << "padAndCenter_width: " << padAndCenter_width << std::endl;
  int centerOffset = 0;
  if (padAndCenter_width > 0) {
    centerOffset = (padAndCenter_width - xPos) / 2;
//    if (centerOffset < padAndCenter_leftPad) {
    if (centerOffset < 0) {
      throw TGenericException(T_SRCANDLINE,
                              "TenmaLineWrapper::applyPadAndCenterToCurrentLine()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": cannot fit on line");
    }
  }
//  std::cerr << "centerOffset: " << centerOffset << std::endl;
//  char c;
//  std::cin >> c;
  temp.writeString(getPadString(padAndCenter_leftPad + centerOffset));
  
  // copy remaining content from original script buffer
  if (currentScriptBuffer.remaining() > 0)
    temp.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
  
  currentScriptBuffer = temp;
}

void TenmaLineWrapper::applyPadAndCenterToCurrentBlock() {
  if (!padAndCenter_on) return;
  if (longestLineLen <= 0) return;
  if (lineContentStartOffsets.size() <= 0) return;

  TBufStream temp;
  
//  std::cerr << longestLineLen << std::endl;
  
  int centerOffset = 0;
  if (padAndCenter_width > 0) {
    centerOffset = (padAndCenter_width - longestLineLen) / 2;
    if ((centerOffset < 0)
        || (longestLineLen > padAndCenter_width)) {
      throw TGenericException(T_SRCANDLINE,
                              "TenmaLineWrapper::applyPadAndCenterToCurrentBlock()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": cannot fit on line");
    }
  }
  
  // copy script buffer up to start of first line
  currentScriptBuffer.seek(0);
  temp.writeFrom(currentScriptBuffer, lineContentStartOffsets[0]);
  
  std::string padString = getPadString(padAndCenter_leftPad + centerOffset);
//  std::cerr << padString << std::endl;
  // awkwardly splice all subsequent lines together with padding applied
  for (int i = 0; i < lineContentStartOffsets.size() - 1; i++) {
    temp.writeString(padString);
    temp.writeFrom(currentScriptBuffer,
      lineContentStartOffsets[i + 1] - lineContentStartOffsets[i]);
  }
  
  // remaining content
  temp.writeString(padString);
  if (currentScriptBuffer.remaining() > 0)
    temp.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
  
  currentScriptBuffer = temp;
}

}
