#include "tenma/TenmaMsgDism.h"
#include "tenma/TenmaMsgDismException.h"
#include "tenma/TenmaMsgConsts.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include "util/TSjis.h"
#include "exception/TGenericException.h"
#include <list>
#include <iostream>

using namespace BlackT;

namespace Pce {


std::string TenmaMsgDismEntry
  ::getStringForm(const TThingyTable& table) const {
  std::string result;
  
  switch (type) {
  case type_string:
  {
    TBufStream ifs;
    ifs.writeString(content);
    ifs.seek(0);
    while (!ifs.eof()) {
      TThingyTable::MatchResult matchResult = table.matchId(ifs);
      if (matchResult.id == -1) {
        throw TGenericException(T_SRCANDLINE,
                                    "TenmaMsgDismEntry::getStringForm()",
                                    "Could not match symbol");
      }
      result += table.getEntry(matchResult.id);
    }
  }
    break;
  case type_literalString:
  {
    result += content;
  }
    break;
  case type_op:
  {
    // op
    if (!table.hasEntry(opNum)) {
      throw TGenericException(T_SRCANDLINE,
                                  "TenmaMsgDismEntry::getStringForm()",
                                  "Could not match op");
    }
    result += table.getEntry(opNum);
    // args
    for (int i = 0; i < content.size(); i++) {
      int value = (unsigned char)content[i];
      result += (std::string("<$")
        + TStringConversion::intToFixedWHexString(value, 2)
        + ">");
    }
  }
    break;
  case type_null:
  {
    result += table.getEntry(0x00);
  }
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                                "TenmaMsgDismEntry::getStringForm()",
                                "Illegal type");
    break;
  }
  
  return result;
}

TenmaMsgDism::TenmaMsgDism()
  : msgType(0),
    msgTypeArg(0),
    msgStartOffset(0),
    msgContentStartOffset(0),
    msgFirstStringStartOffset(-1),
    msgEndOffset(0),
    terminatedByPrompt(false),
    terminatedByF9(false) { }

void TenmaMsgDism::dism(BlackT::TStream& ifs,
          DismMode dismMode,
          MsgTypeMode msgTypeMode,
          bool allowAscii) {
  if (ifs.eof()) {
    throw TenmaMsgDismException(T_SRCANDLINE,
                                "TenmaMsgConsts::TenmaMsgDism::dism()",
                                "out of space");
  }
  
  msgStartOffset = ifs.tell();
  
  // verify presence of valid message type header if requested
  if (msgTypeMode == msgTypeMode_check) {
    msgType = ifs.readu8();
    int argsSize = TenmaMsgConsts::getMsgHeaderArgsSize(msgType);
    if (ifs.remaining() < argsSize) {
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dism()",
                                  "out of space");
    }
    
    if (argsSize > 0) {
      msgTypeArg = ifs.readInt(argsSize,
                      EndiannessTypes::little, SignednessTypes::nosign);
    }
  }
  
  msgContentStartOffset = ifs.tell();
  
  // try to read data
  while (true) {
    if (ifs.eof()) {
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dism()",
                                  "out of space");
    }
    
    int basePos = ifs.tell();
    TenmaMsgDismEntry entry;
    
    bool nullCharsAllowed = false;
    if (dismMode == dismMode_raw) nullCharsAllowed = true;
    
    std::string strContent;
    // must also check if content was actually read to string
    // (null strings don't make sense here)
    if (tryStringRead(ifs, strContent, nullCharsAllowed, allowAscii)
        && (strContent.size() > 0)) {
      entry.type = TenmaMsgDismEntry::type_string;
      entry.content = strContent;
      entries.push_back(entry);
      
      if (msgFirstStringStartOffset == -1)
        msgFirstStringStartOffset = basePos;
      
      continue;
    }
    else {
      ifs.seek(basePos);
    }
    
    // not a string: check for ops
    
    // terminator
    int opNum = ifs.readu8();
    
    if (nullCharsAllowed && allowAscii && (opNum == 0x00)) {
      break;
    }
    
    if (opNum == TenmaMsgConsts::opcode_end) break;
      
    if ((msgFirstStringStartOffset == -1)
        && (opNum == TenmaMsgConsts::opcode_color))
      msgFirstStringStartOffset = basePos;
    
    if (nullCharsAllowed && (opNum == 0x00)) {
//      std::cerr << std::hex << opNum << std::endl;
      entry.type = TenmaMsgDismEntry::type_null;
      entries.push_back(entry);
      continue;
    }
    
    // in raw mode, anything other than a string or terminator is invalid
    // null is also accepted, as it's used for spacing on at least one occasion
    // (the yes/no prompt)
    if (dismMode == dismMode_raw) {
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dism()",
                                  "not valid raw mode string");
    }
    
    if (opNum < TenmaMsgConsts::opcodeBase) {
//      std::cout << basePos << " " << opNum << std::endl;
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dism()",
                                  "not valid string");
    }
    
    int opArgsSize = TenmaMsgConsts::getOpArgsSize(opNum);
    if (ifs.remaining() < opArgsSize) {
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dism()",
                                  "not enough space for op args");
    }
    
    entry.type = TenmaMsgDismEntry::type_op;
    entry.opNum = opNum;
    for (int i = 0; i < opArgsSize; i++) entry.content += ifs.get();
    entries.push_back(entry);
    
    // check for special terminators
    // yes/no prompt
    if (opNum == TenmaMsgConsts::opcode_prompt) {
      terminatedByPrompt = true;
      break;
    }
    else if (opNum == TenmaMsgConsts::opcode_opF9) {
      terminatedByF9 = true;
      break;
    }
    
//    continue;
  }
  
  msgEndOffset = ifs.tell();
}

void TenmaMsgDism::dismMenu(BlackT::TStream& ifs,
                            MenuTypeMode mode) {
  msgContentStartOffset = ifs.tell();
  
  std::list<char> postOpsList;
  while (true) {
    if (ifs.eof()) {
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dismMenu()",
                                  "out of space");
    }
    
    int basePos = ifs.tell();
    TenmaMsgDismEntry entry;
    
//    bool nullCharsAllowed = false;
//    if (dismMode == dismMode_raw) nullCharsAllowed = true;
    
    std::string strContent;
    // must also check if content was actually read to string
    // (null strings don't make sense here)
    if (tryStringRead(ifs, strContent, false, true, true)
        && (strContent.size() > 0)) {
      entry.type = TenmaMsgDismEntry::type_string;
      entry.content = strContent;
      entries.push_back(entry);
    
      // peek for post-param ops check
      for (int i = 0; i < strContent.size(); i++) {
        char next = strContent[i];
        if ((next == '!')
            || (next == '$')
            || (next == '#')) {
          postOpsList.push_back(next);
        }
      }
      
      continue;
    }
    else {
      ifs.seek(basePos);
    }
    
    // not a string: check for ops
    
    // terminator
    int opNum = ifs.readu8();
    
    if (opNum == TenmaMsgConsts::opcode_end) break;
    
    if (opNum < TenmaMsgConsts::opcodeBase) {
//      std::cout << basePos << " " << opNum << std::endl;
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dismMenu()",
                                  "not valid string");
    }
    
    int opArgsSize = TenmaMsgConsts::getOpArgsSize(opNum);
    if (ifs.remaining() < opArgsSize) {
      throw TenmaMsgDismException(T_SRCANDLINE,
                                  "TenmaMsgConsts::TenmaMsgDism::dismMenu()",
                                  "not enough space for op args");
    }
    
    entry.type = TenmaMsgDismEntry::type_op;
    entry.opNum = opNum;
    for (int i = 0; i < opArgsSize; i++) entry.content += ifs.get();
    entries.push_back(entry);
  }
  
  if ((mode == menuTypeMode_params)
      && (!postOpsList.empty())) {
    {
      TenmaMsgDismEntry entry;
      entry.type = TenmaMsgDismEntry::type_literalString;
      entry.content = "[end]\n\n// parameters";
      entries.push_back(entry);
    }
    
    for (auto opEntry: postOpsList) {
      std::string strContent("\n");
      switch (opEntry) {
      case '!':
        for (int i = 0; i < 2; i++) {
          strContent += getHexByte(ifs);
        }
        break;
      case '$':
        for (int i = 0; i < 3; i++) {
          strContent += getHexByte(ifs);
        }
        break;
      case '#':
        for (int i = 0; i < 2; i++) {
          strContent += getHexByte(ifs);
        }
        break;
      default:
        throw TenmaMsgDismException(T_SRCANDLINE,
                                    "TenmaMsgConsts::TenmaMsgDism::dismMenu()",
                                    "bad op");
        break;
      }
      
      TenmaMsgDismEntry entry;
      entry.type = TenmaMsgDismEntry::type_literalString;
      entry.content = strContent;
      entries.push_back(entry);
    }
  }
  
  msgEndOffset = ifs.tell();
}

std::string TenmaMsgDism::getHexByte(BlackT::TStream& ifs) {
  std::string str = TStringConversion::intToString(ifs.readu8(),
    TStringConversion::baseHex).substr(2, std::string::npos);
  while (str.size() < 2) str = std::string("0") + str;
  return std::string("<$") + str + ">";
}

bool TenmaMsgDism::hasStringContent() const {
  for (TenmaMsgDismEntryCollection::const_iterator it = entries.cbegin();
       it != entries.cend();
       ++it) {
    if (it->type == TenmaMsgDismEntry::type_string) return true;
  }
  
  return false;
}

void TenmaMsgDism::stripInitialNonStringContent() {
  TenmaMsgDismEntryCollection oldEntries = entries;
  entries.clear();
  
  TenmaMsgDismEntryCollection::iterator it = oldEntries.begin();
  while (it != oldEntries.end()) {
    if (it->type == TenmaMsgDismEntry::type_string) break;
    if ((it->type == TenmaMsgDismEntry::type_op)
        && (it->opNum == TenmaMsgConsts::opcode_color)) break;
    
    ++it;
  }
  
  while (it != oldEntries.end()) {
    entries.push_back(*it);
    ++it;
  }
  
  msgContentStartOffset = msgFirstStringStartOffset;
}

std::string TenmaMsgDism::getStringForm(const TThingyTable& table,
                            bool noAutoLinebreak) const {
  std::string result;
  
  int lineNum = 0;
  for (TenmaMsgDismEntryCollection::const_iterator it = entries.cbegin();
       it != entries.cend();
       ++it) {
    const TenmaMsgDismEntry& item = *it;
    
    if (item.type == TenmaMsgDismEntry::type_op) {
      int linebreakCount
        = TenmaMsgConsts::numDismPreOpLinebreaks(item.opNum);
      for (int i = 0; i < linebreakCount; i++) {
        result += "\n";
      }
    }
    
    if (!noAutoLinebreak
        && (item.type == TenmaMsgDismEntry::type_op)
        && (item.opNum == TenmaMsgConsts::opcode_br)
        && (lineNum == 2)) {
      // HACK: replace box-breaking linebreaks with an alternate sequence
      // to explicitly notate that a box break should occur here
      // (for use in the translation)
//      result += "\\p";
      result += "#BOXPAUSE()";
    }
    else {
      result += item.getStringForm(table);
    }
    
    if (item.type == TenmaMsgDismEntry::type_op) {
      int linebreakCount
        = TenmaMsgConsts::numDismPostOpLinebreaks(item.opNum);
      for (int i = 0; i < linebreakCount; i++) {
        result += "\n";
      }
      
      if (item.opNum == TenmaMsgConsts::opcode_br) {
//        result += "\n";
        ++lineNum;
        
        // text boxes are automatically broken every three lines
        if (!noAutoLinebreak
            && (lineNum >= 3)) {
          result += "\n";
          lineNum = 0;
        }
      }
    }
  }
  
  // strip trailing linebreaks
  while (result.back() == '\n') result = result.substr(0, result.size() - 1);
  
  return result;
}

TenmaSubStringCollection TenmaMsgDism::getSplitStringForm(
    const BlackT::TThingyTable& table) const {
  TenmaSubStringCollection result;
  
  TenmaMsgDismEntryCollection::const_iterator it = entries.cbegin();
  int lineNum = 0;
  while (it != entries.cend()) {
//    result.push_back(generateNextSubString(entries, it, table));
    generateNextSubStrings(entries, it, table, result, lineNum);
  }
  
  return result;
}

bool TenmaMsgDism::tryStringRead(BlackT::TStream& ifs, std::string& dst,
                                 bool allowNull,
                                 bool allowAscii,
                                 bool noHighAscii) {
  if (ifs.eof()) return false;
  
  while (!ifs.eof()) {
    // check for valid terminating content
    // (any op, including terminator)
    unsigned char first = ifs.get();
    if ((first >= TenmaMsgConsts::opcodeBase)) {
      if (allowAscii && !noHighAscii && (first != 0xFF)) {
        
      }
      else {
        ifs.seekoff(-1);
        return true;
      }
    }
    else if ((allowNull && (first == 0x00))) {
      ifs.seekoff(-1);
      return true;
    }
    
    if (allowAscii
        && ((first < 0x80) || (first >= 0xA0))
        && (!noHighAscii || (first < TenmaMsgConsts::opcodeBase))) {
      dst += first;
      continue;
    }
    
    // if not terminated, this must be a 2-byte sjis code,
    // or string is invalid
    if (ifs.eof()) return false;
    unsigned char second = ifs.get();
    // lazy integer read
    ifs.seekoff(-2);
    int value = ifs.readu16be();
    
    // verify that this is in fact a valid sjis code
    if (!TSjis::isSjis(value)) return false;
    // only values in this range are recognized by the game
    if (value < 0x8000) return false;
    if (value > 0x9FFF) return false;
    
    dst += first;
    dst += second;
  }
  
  return false;
}

void TenmaMsgDism::generateNextSubStrings(
    const TenmaMsgDismEntryCollection& input,
    TenmaMsgDismEntryCollection::const_iterator& it,
    const BlackT::TThingyTable& table,
    TenmaSubStringCollection& dst,
    int& lineNum) const {
  TenmaSubString result;
//  int lineNum = 0;
  bool typeSet = false;
  bool needsAutoPause = false;
  int pendingNumLinebreaks = 0;
//  bool colorsNotInlineable = false;
//  bool startsWithColor = true;
  bool prefixNextColor = true;
  bool fullyTerminated = false;
  bool cleared = false;
  int startingLineNum = lineNum;
  while (it != input.cend()) {
    const TenmaMsgDismEntry& item = *it;
    
    if (!typeSet) {
      if (item.type == TenmaMsgDismEntry::type_op) {
    
        // HACK: do not allow initial [color] commands to be inlined.
        // nearly all [color] commands apply to an entire box of text,
        // and there's no point in having to manually copy every one over
        // for the translation.
//        if (item.opNum == TenmaMsgConsts::opcode_color) {
//          result.visible = false;
//          colorsNotInlineable = true;
//        }
//        else 
        if (TenmaMsgConsts::isInlineableOp(item.opNum)) {
          result.visible = true;
        }
        else {
          result.visible = false;
        }
        
        if (item.opNum == TenmaMsgConsts::opcode_color)
          prefixNextColor = true;
      }
      else {
        // literal
        result.visible = true;
      }
      
      typeSet = true;
      continue;
    }
    else {
      if (item.type == TenmaMsgDismEntry::type_op) {
        bool isInlineable = TenmaMsgConsts::isInlineableOp(item.opNum);
//        if (colorsNotInlineable
//            && (item.opNum == TenmaMsgConsts::opcode_color))
//          isInlineable = false;
        
        if ((item.opNum == TenmaMsgConsts::opcode_end)
            || (item.opNum == TenmaMsgConsts::opcode_prompt)) {
          fullyTerminated = true;
        }
        else if (item.opNum == TenmaMsgConsts::opcode_clear) {
          cleared = true;
        }
        
        if (result.visible && !isInlineable) {
          break;
        }
        else if (!result.visible && isInlineable) {
          break;
        }
      }
      else {
        // literal
        if (!result.visible) {
          break;
        }
      }
    }
    
    ++it;
    
    if (prefixNextColor
        && (item.type == TenmaMsgDismEntry::type_op)
        && (item.opNum == TenmaMsgConsts::opcode_color)) {
      result.prefix += item.getStringForm(table);
      prefixNextColor = false;
      continue;
    }
    
    if (item.type == TenmaMsgDismEntry::type_op) {
      int linebreakCount
        = TenmaMsgConsts::numDismPreOpLinebreaks(item.opNum);
      for (int i = 0; i < linebreakCount; i++) {
        result.content += "\n";
      }
    }
    
    if ((item.type == TenmaMsgDismEntry::type_op)
        && (item.opNum == TenmaMsgConsts::opcode_br)) {
//      ++pendingNumLinebreaks;
    }
    else {
      if (pendingNumLinebreaks > 0) {
        if ((item.type == TenmaMsgDismEntry::type_op
                && TenmaMsgConsts::isInlineableOp(item.opNum))
             || (item.type == TenmaMsgDismEntry::type_string)
             ) {
          outputLinebreaks(result, table, pendingNumLinebreaks);
//          lineNum += pendingNumLinebreaks;
          pendingNumLinebreaks = 0;
        }
      }
    }
    
    if ((item.type == TenmaMsgDismEntry::type_op)
        && (item.opNum == TenmaMsgConsts::opcode_br)
        && (lineNum == 2)) {
      // HACK: replace box-breaking linebreaks with an alternate sequence
      // to explicitly notate that a box break should occur here
      // (for use in the translation)
//      result.content += "\\p";
      needsAutoPause = true;
      break;
    }
    else if ((item.type == TenmaMsgDismEntry::type_op)
        && (item.opNum == TenmaMsgConsts::opcode_br)) {
    
    }
    else {
      result.content += item.getStringForm(table);
    }
    
    if (item.type == TenmaMsgDismEntry::type_op) {
      if (item.opNum != TenmaMsgConsts::opcode_br) {
        int linebreakCount
          = TenmaMsgConsts::numDismPostOpLinebreaks(item.opNum);
        
//        if (startsWithColor && (item.opNum == TenmaMsgConsts::opcode_color)) {
//          linebreakCount = 1;
//          startsWithColor = false;
//        }
        
        for (int i = 0; i < linebreakCount; i++) {
          result.content += "\n";
        }
      }
      
      if (item.opNum == TenmaMsgConsts::opcode_br) {
//        result.content += "\n";
        ++lineNum;
        ++pendingNumLinebreaks;
        
        // text boxes are automatically broken every three lines
/*        if (lineNum >= 3) {
          result.content += "\n";
          lineNum = 0;
        }*/
      }
    }
  }
  
  if (it == input.cend())
    fullyTerminated = true;
  
  if (!needsAutoPause) {
    // only show trailing linebreaks if the box is not filled
    if (lineNum < 2) {
      if (pendingNumLinebreaks > 0) {
        outputLinebreaks(result, table, pendingNumLinebreaks);
  //      lineNum += pendingNumLinebreaks;
        pendingNumLinebreaks = 0;
      }
    }
  }
  
  // strip trailing linebreaks
  while (result.content.back() == '\n')
    result.content = result.content.substr(0, result.content.size() - 1);
  
  if ((result.visible)
      && (result.content.size() > 0)
      && (startingLineNum != 0)) {
    result.content = std::string("// WARNING: continues previous box\n")
      + result.content;
  }
  
  if (result.content.size() > 0) {
    dst.push_back(result);
  }
  
  if (needsAutoPause) {
    TenmaSubString result;
    result.visible = false;
//    result.content = "\\p";
    result.content = "#BOXPAUSE()";
    dst.push_back(result);
    lineNum = 0;
  }
  else {
    if (fullyTerminated && (lineNum >= 2)) {
      // deal with the case where the box is full but is not pause-broken
      // (usually for last message, which is automatically paused when
      // the terminator is reached)
      TenmaSubString result;
      result.visible = false;
//      result.content = "\\t";
      result.content = "#BOXEND()";
      dst.push_back(result);
    }
  }
  
  if (cleared) lineNum = 0;
}

void TenmaMsgDism::outputLinebreaks(TenmaSubString& dst,
    const BlackT::TThingyTable& table, int count) {
  TenmaMsgDismEntry brEntry;
  brEntry.type = TenmaMsgDismEntry::type_op;
  brEntry.opNum = TenmaMsgConsts::opcode_br;
  std::string output = brEntry.getStringForm(table);
  for (int i = 0; i < count; i++) {
    dst.content += output;
    dst.content += "\n";
  }
}


}
