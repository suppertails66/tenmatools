#include "util/TLineWrapper.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>

namespace BlackT {


// for testing purposes
#define TLINEWRAPPER_IGNORE_BAD_INPUT 0


const static int scriptBufferCapacity = 0x100;
const static int wordBufferCapacity = 0x100;

TLineWrapper::TLineWrapper(
                  BlackT::TStream& src__,
                  ResultCollection& dst__,
                  const BlackT::TThingyTable& thingy__,
                  int xSize__,
                  int ySize__)
  : src(src__),
    dst(dst__),
    thingy(thingy__),
    lineNum(0),
    xSize(xSize__),
    ySize(ySize__),
    xPos(0),
    yPos(0),
    currentWordWidth(0),
    maxCurrentWordWidth(0),
    pendingAdvanceWidth(0),
    nonDividerEncountered(false),
    lineHasContent(false),
    copyEverythingLiterally(false),
    currentLineContentStartInBuffer(-1),
    currentScriptBuffer(scriptBufferCapacity),
    currentWordBuffer(wordBufferCapacity) {
//  loadThingy(thingy__);
}

void TLineWrapper::operator()() {
  while (!src.eof()) {
    std::string line;
    src.getLine(line);
    ++lineNum;
    
//    std::cerr << lineNum << std::endl;
//    std::cerr << streamAsString(currentScriptBuffer) << std::endl;
    
    if (line.size() <= 0) continue;
    
    // discard lines containing only ASCII spaces and tabs
//    bool onlySpace = true;
//    for (int i = 0; i < line.size(); i++) {
//      if ((line[i] != ' ')
//          && (line[i] != '\t')) {
//        onlySpace = false;
//        break;
//      }
//    }
//    if (onlySpace) continue;
    
    try {
      TBufStream ifs(line.size());
      ifs.write(line.c_str(), line.size());
      ifs.seek(0);
      
      // check for special stuff
      if (ifs.peek() == '#') {
        // directives
        ifs.get();
        
        // if derived class handled directive, don't process it ourself
        int pos = ifs.tell();
        if (processUserDirective(ifs)) continue;
        ifs.seek(pos);
        
        processDirective(ifs);
        continue;
      }
      
      while (!ifs.eof()) {
        // check for comments
        if ((ifs.remaining() >= 2)
            && (ifs.peek() == '/')) {
          ifs.get();
          if (ifs.peek() == '/') {
  /*          ifs.unget();
            std::string remaining;
            while (!ifs.eof()) remaining += ifs.get();
            flushActiveWord();
            currentScriptBuffer.write(remaining.c_str(), remaining.size());
            currentScriptBuffer.put('\n'); */
            break;
          }
          else ifs.unget();
        }
        
        if (copyEverythingLiterally) {
          currentScriptBuffer.put(ifs.get());
        }
        else {
          outputNextSymbol(ifs);
        }
      }
    }
    catch (TGenericException& e) {
      std::cerr << "Script line " << lineNum << ":" << std::endl;
      std::cerr << "Exception: " << std::endl;
      std::cerr << e.problem() << std::endl;
      throw e;
    }
    catch (std::exception& e) {
      std::cerr << "Script line " << lineNum << ":" << std::endl;
      std::cerr << "  Exception: " << std::endl;
      std::cerr << e.what() << std::endl;
      throw e;
    }
  }
  
  // if we hit EOF without encountering an END(), output any remaining
  // content
  if (currentScriptBuffer.size() > 0) {
    flushActiveScript();
  }
}

//int TLineWrapper::linebreakHeight() const {
//  return 1;
//}
  
int TLineWrapper::widthOfKey(int key) { return 0; }
  
int TLineWrapper::glyphWidthOfKey(int key) { return widthOfKey(key); }

int TLineWrapper::advanceWidthOfKey(int key) { return 0; }

bool TLineWrapper::isVisibleWordDivider(int key, int nextKey) {
  return false;
}

void TLineWrapper::onSymbolAdded(BlackT::TStream& ifs, int key) {
  
}

void TLineWrapper::afterSymbolAdded(BlackT::TStream& ifs, int key) {
  
}

void TLineWrapper::onLineContentStarted() {
  
}

void TLineWrapper::beforeLinebreak(
    LinebreakSource clearSrc, int key) {
  
}

void TLineWrapper::afterLinebreak(
    LinebreakSource clearSrc, int key) {
  
}

void TLineWrapper::beforeBoxClear(
    BoxClearSource clearSrc, int key) {
  
}

void TLineWrapper::afterBoxClear(BoxClearSource clearSrc,
                             int key) {
  
}

void TLineWrapper::beforeWordFlushed() {
  
}

void TLineWrapper::loadThingy(const BlackT::TThingyTable& thingy__) {
  thingy = thingy__;
}

TThingyTable::MatchResult TLineWrapper::getSymbolId(BlackT::TStream& ifs) {
  // check if a literal value
//  if ((ifs.remaining() >= literalSize)
//      && (ifs.peek() == '<')) {
    if (ifs.peek() == '<') {
    int pos = ifs.tell();
    
    std::string litstr;
    
    litstr += ifs.get();
    if (!ifs.eof() && ifs.peek() == '$') {
      litstr += ifs.get();
      
      std::string valuestr = "0x";
      
      while (!ifs.eof() && ifs.peek() != '>') valuestr += ifs.get();
//      valuestr += ifs.get();
      
      if (ifs.peek() == '>') {
        litstr += ifs.get();
//        currentWordBuffer.write(litstr.c_str(), litstr.size());
        TThingyTable::MatchResult result;
        
//        int value = TStringConversion::intToString(valuestr);
//        result.id = value;
//        result.size = 5;

//        result.id = -1;
//        result.size = -1;

        result.id = -1;
        result.size = litstr.size();
        return result;
      }
    }
    
    // not a literal value
    ifs.seek(pos);
  }
  
  return thingy.matchTableEntry(ifs);
}

void TLineWrapper::outputNextSymbol(TStream& ifs) {
  Symbol result = fetchNextSymbol(ifs);
  
  // we know nothing about raw data; just copy to buffer and return
  if (result.raw) {
    currentWordBuffer.write(result.litstr.c_str(), result.litstr.size());
    return;
  }
  
  int id = result.id;

  #if TLINEWRAPPER_IGNORE_BAD_INPUT
    if (id == -1) {
      // ignore unrecognized characters
      // (and handle sjis sequences)
//      if (((ifs.readu8() & 0x80) != 0) && !ifs.eof()) ifs.get();
      ifs.get();
      return;
    }
  #endif
  
//  std::cerr << std::hex << id << " " << std::dec << currentWordWidth << " " << xPos << " " << yPos << std::endl;
//  char c; std::cin >> c;
  
//  std::cerr << id << std::endl;
  if (id != -1) {
//    int symbolSize;
//    if (result.id <= 0xFF) symbolSize = 1;
//    else if (result.id <= 0xFFFF) symbolSize = 2;
//    else if (result.id <= 0xFFFFFF) symbolSize = 3;
//    else symbolSize = 4;
    
//    currentScriptBuffer.writeInt(result.id, symbolSize,
//      EndiannessTypes::big, SignednessTypes::nosign);
    
    bool isDivider = isWordDivider(id);
    
    int nextId = -1;
    if (!ifs.eof()) {
      int pos = ifs.tell();
      Symbol result = fetchNextSymbol(ifs);
      nextId = result.id;
      ifs.seek(pos);
    }
    
    bool isVisibleDivider = isVisibleWordDivider(id, nextId);
    
    // notify derived classes
    onSymbolAdded(ifs, id);
    
    if (isLinebreak(id)) {
      // output current word
      flushActiveWord();
      
      beforeLinebreak(linebreakManual, id);
      
      // add linebreak
//      outputLinebreak(thingy.getEntry(id));
      handleManualLinebreak(result, id);
//      outputLinebreak(result.getContent(thingy));
      
//      std::cerr << streamAsString(currentScriptBuffer) << std::endl;
//      std::cerr << xPos << " " << yPos << std::endl;
      
      afterLinebreak(linebreakManual, id);
      
      pendingAdvanceWidth = 0;
      
//      return;
    }
    else if (isBoxClear(id)) {
      // output current word
      flushActiveWord();
      
      beforeBoxClear(boxClearManual, id);
      
//      std::string content = thingy.getEntry(id);
      std::string content = result.getContent(thingy);
      currentScriptBuffer.write(content.c_str(), content.size());
      currentScriptBuffer.put('\n');
      currentScriptBuffer.put('\n');
        
      xPos = 0;
      yPos = 0;
      pendingAdvanceWidth = 0;
      
      afterBoxClear(boxClearManual, id);
//      return;
    }
    else {
      if ((isDivider && nonDividerEncountered)) {
        // output current word
        
        // add pending advance width (which must apply, since we're adding
        // more characters)
//        std::cerr << "here: " << currentWordWidth << " " << maxCurrentWordWidth << " " << pendingAdvanceWidth << std::endl;
        currentWordWidth += pendingAdvanceWidth;
        if (currentWordWidth > maxCurrentWordWidth)
          maxCurrentWordWidth = currentWordWidth;
        
        flushActiveWord();
        pendingAdvanceWidth = 0;
      }
      
      // add pending advance width from previous character
      currentWordWidth += pendingAdvanceWidth;
      pendingAdvanceWidth = 0;
      if (currentWordWidth > maxCurrentWordWidth)
        maxCurrentWordWidth = currentWordWidth;
      
      int glyphWidth = glyphWidthOfKey(id);
      int advanceWidth = advanceWidthOfKey(id);
    
      // add content to current word
      currentWordWidth += glyphWidth;
      if (currentWordWidth > maxCurrentWordWidth)
        maxCurrentWordWidth = currentWordWidth;
      
      // set up new advance width
      pendingAdvanceWidth = advanceWidth - glyphWidth;
  
//  std::cerr << "xPos: " << xPos << " currentWordWidth: " << currentWordWidth << " maxCurrentWordWidth: " << maxCurrentWordWidth << " glyphWidth: " << glyphWidth << " advanceWidth: " << advanceWidth << " pendingAdvanceWidth: " << pendingAdvanceWidth << std::endl;
      
  //    std::string content = thingy.getEntry(id);
      std::string content = result.getContent(thingy);
      currentWordBuffer.write(content.c_str(), content.size());
      
      if (!isDivider) {
        nonDividerEncountered = true;
      }
      
      // if a visible divider, immediately flush
      if (isVisibleDivider) {
        flushActiveWord();
        // FIXME?
//        pendingAdvanceWidth = 0;
      }
      
    
//    return;
    }
  }
  else {
    std::string remainder;
    ifs.getLine(remainder);
    throw TGenericException(T_SRCANDLINE,
                            "TLineWrapper::outputNextSymbol()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ":\n  Couldn't match symbol at: '"
                              + remainder
                              + "'");
  }
  
  // notify derived classes
  afterSymbolAdded(ifs, id);
  
/*  std::string remainder;
  ifs.getLine(remainder);
  
  // if we reached end of file, this is not an error: we're done
  if (ifs.eof()) return;
  
  throw TGenericException(T_SRCANDLINE,
                          "TLineWrapper::outputNextSymbol()",
                          "Line "
                            + TStringConversion::intToString(lineNum)
                            + ":\n  Couldn't match symbol at: '"
                            + remainder
                            + "'"); */
}

TLineWrapper::Symbol TLineWrapper::fetchNextSymbol(BlackT::TStream& ifs) {
  Symbol result;
  result.id = -1;
  result.raw = false;
  
  // literal value
//  if ((ifs.remaining() >= literalSize)
//      && (ifs.peek() == '<')) {
  if (ifs.peek() == '<') {
    int pos = ifs.tell();
    
    std::string litstr;
    
    litstr += ifs.get();
    if (!ifs.eof() && ifs.peek() == '$') {
      litstr += ifs.get();
//      std::string valuestr = "0x";
//      valuestr += ifs.get();
//      valuestr += ifs.get();

      while (!ifs.eof() && ifs.peek() != '>')
        litstr += ifs.get();
//      litstr += ifs.get();
      
      if (!ifs.eof() && ifs.peek() == '>') {
        litstr += ifs.get();
//        currentWordBuffer.write(litstr.c_str(), litstr.size());
        
        result.raw = true;
        result.litstr = litstr;
        return result;
      }
    }
    
    // not a literal value
    ifs.seek(pos);
  }
  
  TThingyTable::MatchResult matchResult;
  matchResult = thingy.matchTableEntry(ifs);
  result.id = matchResult.id;
  
  return result;
}

void TLineWrapper
    ::handleManualLinebreak(TLineWrapper::Symbol result, int key) {
  outputLinebreak(result.getContent(thingy));
}

void TLineWrapper::outputLinebreak() {
//  std::string content = thingy.getEntry(linebreakKey());
//  outputLinebreak(content);
  outputLinebreak(linebreakString());
}

void TLineWrapper::outputLinebreak(std::string str) {
  // check if y-limit will be exceeded
  if ((ySize != -1) && yPos >= ySize - 1) {
    // clear box
    beforeBoxClear(boxClearBoxFull, -1);
    onBoxFull();
    afterBoxClear(boxClearBoxFull, -1);
    
    // do not output a linebreak; the box clear has taken care of it
    return;
 }
  
  // flush any literals at start of word; otherwise, they'll erroneously
  // get inserted after the injected linebreak
  // (folded into stripCurrentPreDividers())
//  flushCurrentPreLiterals();
  
  stripCurrentPreDividers();
  
  currentScriptBuffer.write(str.c_str(), str.size());
  currentScriptBuffer.put('\n');
  
  xPos = 0;
  ++yPos;
//  yPos += linebreakHeight();
  lineHasContent = false;
  currentLineContentStartInBuffer = -1;
}

void TLineWrapper::flushCurrentPreLiterals() {
  TBufStream newWordBuf = TBufStream(wordBufferCapacity);
  currentWordBuffer.seek(0);
  
  while (!currentWordBuffer.eof()) {
    // getNextSymbol()
    TThingyTable::MatchResult result = getSymbolId(currentWordBuffer);
    
    // literal
    if (result.id == -1) {
      // flush
//      currentWordBuffer.seekoff(-literalSize);
//      currentScriptBuffer.writeFrom(currentWordBuffer, literalSize);
      currentWordBuffer.seekoff(-result.size);
      currentScriptBuffer.writeFrom(currentWordBuffer, result.size);
    }
    // done at first nonliteral
    else {
      currentWordBuffer.seekoff(-result.size);
      break;
    }
  }
  
  newWordBuf.writeFrom(currentWordBuffer,
    currentWordBuffer.size() - currentWordBuffer.tell());
  currentWordBuffer = newWordBuf;
}

void TLineWrapper::stripCurrentPreDividers() {
  // flush any literals at start of word; otherwise, they'll erroneously
  // get inserted after the injected linebreak
  flushCurrentPreLiterals();

  // remove any word dividers from start of current word
  TBufStream newWordBuf = TBufStream(wordBufferCapacity);
  currentWordBuffer.seek(0);
//  std::cerr << "buf: '" << streamAsString(currentWordBuffer) << "'" << std::endl;
//  std::cerr << "width: " << currentWordWidth << std::endl;
  while (!currentWordBuffer.eof()) {
    // getNextSymbol()
    TThingyTable::MatchResult result = getSymbolId(currentWordBuffer);
    
    // if not a word divider, break
    if (result.id == -1) {
      // literal
//      currentWordBuffer.seekoff(-literalSize);
      currentWordBuffer.seekoff(-result.size);
      break;
    }
    else if (!isWordDivider(result.id)) {
      currentWordBuffer.seekoff(-result.size);
      break;
    }
    else {
      // otherwise, subtract size of divider from currentWordWidth
      // FIXME: word dividers cannot have kerning or this will produce
      // incorrect results
      // FIXME: inter-word kerning is fucked
      int advanceWidth = advanceWidthOfKey(result.id);
      currentWordWidth -= advanceWidth;
      maxCurrentWordWidth -= advanceWidth;
    }
//    std::cerr << currentWordWidth << std::endl;
  }
  newWordBuf.writeFrom(currentWordBuffer,
    currentWordBuffer.size() - currentWordBuffer.tell());
  currentWordBuffer = newWordBuf;
}

int TLineWrapper::fromLitString(const std::string& litstr) {
  std::string str = "0x" + litstr.substr(2, 2);
  return TStringConversion::stringToInt(str);
}

void TLineWrapper::flushActiveScript() {
  if (currentWordBuffer.size() > 0) flushActiveWord();

  int outputSize = currentScriptBuffer.size();
  
  ResultString result;
  currentScriptBuffer.seek(0);
  while (!currentScriptBuffer.eof()) {
    result.str += currentScriptBuffer.get();
  }
  
  dst.push_back(result);
  
  // clear script buffer
  currentScriptBuffer = TBufStream(scriptBufferCapacity);
  xPos = 0;
  yPos = 0;
  currentWordWidth = 0;
  maxCurrentWordWidth = 0;
  pendingAdvanceWidth = 0;
  nonDividerEncountered = false;
  lineHasContent = false;
  currentLineContentStartInBuffer = -1;
}

void TLineWrapper::flushActiveWord() {
  if (currentWordBuffer.size() == 0) return;

  // will word fit on current line?
  if ((xSize == -1) || ((xPos + maxCurrentWordWidth) <= xSize)) {
    
  }
  else {
    // check if y-limit will be exceeded
    if ((ySize != -1) && (yPos >= (ySize - 1))) {
      beforeBoxClear(boxClearBoxFull, -1);
      onBoxFull();
      afterBoxClear(boxClearBoxFull, -1);
    }
    
    // check if box full handler means we no longer need a linebreak
    if ((xSize == -1) || ((xPos + maxCurrentWordWidth) > xSize)) {
      beforeLinebreak(linebreakLineFull, -1);
      outputLinebreak();
      afterLinebreak(linebreakLineFull, -1);
    }
    
    // check if word is too long to fit on one line
    if ((xPos + maxCurrentWordWidth) > xSize) {
      std::cerr << "WARNING: Line " << lineNum << ":" << std::endl;
      std::cerr << "  Word too long to fit on one line: '";
      std::cerr << streamAsString(currentWordBuffer);
      std::cerr << "  Width limit is " << xSize
        << ", actual width " << xPos + maxCurrentWordWidth
        << " (-advanceW), placement position " << xPos + currentWordWidth
        << ", overflow " << ((xPos + maxCurrentWordWidth) - xSize) << std::endl;
//      currentWordBuffer.seek(0);
//      while (!currentWordBuffer.eof()) {
//        std::cerr << currentWordBuffer.get();
//      }
      std::cerr << "'" << std::endl;
    }
  }
  
  if ((ySize != -1) && (yPos >= ySize)) {
//      std::cerr << "scr:  " << streamAsString(currentScriptBuffer) << std::endl;
//      std::cerr << "word: " << streamAsString(currentWordBuffer) << std::endl
//         << std::endl << std::endl;
    onBoxFull();
  }
  
  // output word
  if (!lineHasContent) {
//    std::cerr << "start: " << currentLineContentStartInBuffer << std::endl;
    currentLineContentStartInBuffer = currentScriptBuffer.tell();
    onLineContentStarted();
//    std::cerr << "end: " << currentLineContentStartInBuffer << std::endl;
  }
  
  beforeWordFlushed();
  
  currentWordBuffer.seek(0);
  currentScriptBuffer.writeFrom(currentWordBuffer, currentWordBuffer.size());
  xPos += maxCurrentWordWidth;
  currentWordWidth = 0;
  maxCurrentWordWidth = 0;
  nonDividerEncountered = false;
  lineHasContent = true;
//  std::cerr << xPos << std::endl;
  
  // clear buffer
  currentWordBuffer = TBufStream(wordBufferCapacity);
}

std::string TLineWrapper::streamAsString(TStream& stream) {
  int pos = stream.tell();
  stream.seek(0);
  std::string result;
  while (!stream.eof()) result += stream.get();
  stream.clear();
  stream.seek(pos);
  return result;
}
  
/*bool TLineWrapper::checkSymbol(BlackT::TStream& ifs, std::string& symbol) {
  if (symbol.size() > ifs.remaining()) return false;
  
  int startpos = ifs.tell();
  for (int i = 0; i < symbol.size(); i++) {
    if (symbol[i] != ifs.get()) {
      ifs.seek(startpos);
      return false;
    }
  }
  
  return true;
} */

bool TLineWrapper::processUserDirective(BlackT::TStream& ifs) {
  return false;
}
  
void TLineWrapper::processDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("LOADTABLE") == 0) {
    processLoadTable(ifs);
    
    // copy directive to output
    ifs.seek(0);
    currentScriptBuffer.put('\n');
    currentScriptBuffer.writeFrom(ifs, ifs.size());
    currentScriptBuffer.put('\n');
    return;
  }
//  else if (name.compare("END") == 0) {
//    processEndMsg(ifs);
//    return;
//  }
  else if (name.compare("SETWIDTH") == 0) {
    processSetWidth(ifs);
  }
  else if (name.compare("SETHEIGHT") == 0) {
    processSetHeight(ifs);
  }
  else if (name.compare("SETSIZE") == 0) {
    processSetSize(ifs);
  }
  else {
//    throw TGenericException(T_SRCANDLINE,
//                            "TLineWrapper::processDirective()",
//                            "Line "
//                              + TStringConversion::intToString(lineNum)
//                              + ":\n  Unknown directive: "
//                              + name);
    
    // copy directive to output
    flushActiveWord();
    ifs.seek(0);
    currentScriptBuffer.put('\n');
    currentScriptBuffer.writeFrom(ifs, ifs.size());
    currentScriptBuffer.put('\n');
    
    return;
  }
  
  TParse::matchChar(ifs, ')');
}

void TLineWrapper::processLoadTable(BlackT::TStream& ifs) {
  std::string tableName = TParse::matchString(ifs);
  TThingyTable table(tableName);
  loadThingy(table);
}

void TLineWrapper::processEndMsg(BlackT::TStream& ifs) {
  flushActiveWord();

  // copy directive to output
  int pos = ifs.tell();
  ifs.seek(0);
  currentScriptBuffer.put('\n');
  currentScriptBuffer.writeFrom(ifs, ifs.size());
  currentScriptBuffer.put('\n');
  ifs.seek(pos);
  
  flushActiveScript();
}

//void TLineWrapper::processIncBin(BlackT::TStream& ifs) {
//  std::string filename = TParse::matchString(ifs);
//  TBufStream src(1);
//  src.open(filename.c_str());
//  currentScriptBuffer.writeFrom(src, src.size());
//}

void TLineWrapper::processSetWidth(BlackT::TStream& ifs) {
  xSize = TParse::matchInt(ifs);
}

void TLineWrapper::processSetHeight(BlackT::TStream& ifs) {
  ySize = TParse::matchInt(ifs);
}

void TLineWrapper::processSetSize(BlackT::TStream& ifs) {
  xSize = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  ySize = TParse::matchInt(ifs);
}


}
