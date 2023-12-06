#include "tenma/TenmaScriptGenStream.h"
#include "tenma/TenmaMsgConsts.h"
#include "tenma/TenmaMsgDism.h"
#include "tenma/TenmaMsgDismException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


TenmaScriptGenStreamEntry::TenmaScriptGenStreamEntry()
  : type(type_none),
    offset(-1),
    size(-1),
    mapSectorNum(-1),
    mayNotExist(false) { }

TenmaScriptGenStream::TenmaScriptGenStream() { }

void TenmaScriptGenStream::exportToSheet(
    TenmaTranslationSheet& dst,
    std::ostream& ofs,
    std::string idPrefix) const {
  
//  int strNum = 0;
  for (unsigned int i = 0; i < entries.size(); i++) {
    const TenmaScriptGenStreamEntry& item = entries[i];
    
    if ((item.type == TenmaScriptGenStreamEntry::type_string)
//        || (item.type == TenmaScriptGenStreamEntry::type_mapString)
        ) {
      std::string idString = idPrefix
//          + TStringConversion::intToString(strNum)
//          + "-"
        + TStringConversion::intToString(entries[i].offset,
            TStringConversion::baseHex);
      if (!item.idOverride.empty()) idString = item.idOverride;
      
      ofs << "#STARTSTRING("
        << "\"" << idString << "\""
//        << ", "
//        << TStringConversion::intToString(item.offset,
//            TStringConversion::baseHex)
//        << ", "
//        << TStringConversion::intToString(item.size,
//            TStringConversion::baseHex)
        << ")" << std::endl;
      
      if (item.offset != -1) {
        ofs << "#SETORIGINALPOS("
          << TStringConversion::intToString(item.offset,
              TStringConversion::baseHex)
          << ", "
          << TStringConversion::intToString(item.size,
              TStringConversion::baseHex)
          << ")" << std::endl;
      }
      
      for (const auto& propertyPair: item.propertyMap) {
        ofs << "#SETSTRINGPROPERTY("
          << "\"" << propertyPair.first << "\""
          << ", \"" << propertyPair.second << "\""
          << ")"
          << std::endl;
      }
      
      if (item.subStrings.empty()) {
        dst.addStringEntry(
          idString, item.content, "", "", item.translationPlaceholder);
        if (item.mayNotExist)
          ofs << "#IMPORTIFEXISTS(\"";
        else
          ofs << "#IMPORT(\"";
        
        ofs << idString << "\")" << std::endl;
      }
      else {
        for (int i = 0; i < item.subStrings.size(); i++) {
          const TenmaSubString& subItem = item.subStrings[i];
          
          if (!subItem.visible) {
            ofs << subItem.content << std::endl;
            continue;
          }
          
          std::string subId = idString
            + "_" + TStringConversion::intToString(i);
          dst.addStringEntry(
            subId, subItem.content, subItem.prefix, "", subItem.translationPlaceholder);
          
          if (item.mayNotExist)
            ofs << "#IMPORTIFEXISTS(\"";
          else
            ofs << "#IMPORT(\"";
          ofs << subId << "\")" << std::endl;
        }
      }
      
      ofs << "#ENDSTRING()" << std::endl;
      ofs << std::endl;
      
//      ++strNum;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_setRegion) {
      ofs << "#STARTREGION("
//        << item.regionId
        << "\"" << item.content << "\""
        << ")" << std::endl;
      ofs << std::endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_setMap) {
      ofs << "#SETMAP("
        << item.mapSectorNum
        << ")" << std::endl;
      ofs << std::endl;
    }
/*    else if (item.type == TenmaScriptGenStreamEntry::type_setNotCompressible) {
      ofs << "#SETNOTCOMPRESSIBLE("
        << (item.notCompressible ? 1 : 0)
        << ")" << endl;
      ofs << endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_addOverwrite) {
      ofs << "#ADDOVERWRITE("
        << TStringConversion::intToString(item.offset,
          TStringConversion::baseHex)
        << ")" << endl;
      ofs << endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_genericLine) {
      ofs << item.content << endl;
      ofs << endl;
    } */
    else if (item.type == TenmaScriptGenStreamEntry::type_comment) {
      dst.addCommentEntry(item.content);
      
      ofs << "//===================================" << std::endl;
      ofs << "// " << item.content << std::endl;
      ofs << "//===================================" << std::endl;
      ofs << std::endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_smallComment) {
      dst.addSmallCommentEntry(item.content);
      
      ofs << "//=====" << std::endl;
      ofs << "// " << item.content << std::endl;
      ofs << "//=====" << std::endl;
      ofs << std::endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_marker) {
      dst.addMarkerEntry(item.content);
      
      ofs << "// === MARKER: " << item.content << std::endl;
      ofs << std::endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_genericLine) {
      ofs << item.content << std::endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_setRegionProp) {
      ofs << "#SETREGIONPROPERTY("
        << "\"" << item.propName << "\""
        << ", \"" << item.propValue << "\""
        << ")"
        << std::endl << std::endl;
    }
    else if (item.type == TenmaScriptGenStreamEntry::type_setStringProp) {
      ofs << "#SETSTRINGPROPERTY("
        << "\"" << item.propName << "\""
        << ", \"" << item.propValue << "\""
        << ")"
        << std::endl;
    }
  }
}

void TenmaScriptGenStream::addMapFromIsoSector(TStream& iso, int sectorNum,
                         TenmaMapIndex& mapIndex,
                         const TThingyTable& table,
                         int flags) {
  iso.seek(sectorNum * TenmaMsgConsts::sectorSize);
  TBufStream ifs;
  
  int mapBasePos = iso.tell();
  // read size field from header
//  iso.seek(mapBasePos + 0x12);
//  int endStrPtr = iso.readu16le();

  // TODO: this is potentially a bad assumption (what if the
  // pointed-to content extends into the next sector?).
  // i believe this should catch all strings, though.
  // probably.
  iso.seek(mapBasePos);
  int biggestPtr = 0;
  for (int i = 0; i < 14; i++) {
    int next = iso.readu16le();
    if (next > biggestPtr) biggestPtr = next;
  }
  
  // this is not quite correct but it's probably good enough
  iso.seek(mapBasePos + 26);
//  iso.seek(mapBasePos + 20);
  int textStartPtr = iso.readu16le();
  iso.seek(mapBasePos + 22);
  int mapNamePtr = iso.readu16le();
  
  iso.seek(mapBasePos);
  if (biggestPtr == 0) {
    ifs.writeFrom(iso, TenmaMsgConsts::mapMsgBlockSize);
  }
  else {
/*    int endOffset = (biggestPtr - 0xC000) + 1;
    int sectorCount = ((endOffset + (TenmaMsgConsts::sectorSize - 1))
                        / TenmaMsgConsts::sectorSize);
//    if (sectorCount > TenmaMsgConsts::mapMsgBlockSize)
//      sectorCount = TenmaMsgConsts::mapMsgBlockSize;*/
    
//    int sectorCount
//      = (TenmaMsgConsts::mapMsgBlockSize / TenmaMsgConsts::sectorSize);
    
    // somebody could not be assed to initialize their fucking buffers
    // while assembling the maps, resulting in unused sectors being
    // filled with duplicates from previous maps and leaving us to
    // have to figure out where the actual data block ends.
    // otherwise, we'll end up dumping a bunch of random stuff repeated
    // from some other map in the block.
    int endOffset = (biggestPtr - 0xC000) + 1;
    int sectorCount = ((endOffset + (TenmaMsgConsts::sectorSize - 1))
                        / TenmaMsgConsts::sectorSize);
    while (sectorCount < TenmaMsgConsts::mapMsgBlockSectorSize) {
      iso.seek(mapBasePos + (sectorCount * TenmaMsgConsts::sectorSize) - 1);
      bool good = true;
      // check that last N bytes are zero
      // (obviously there will be problems if valid content happens to stop
      // close to a sector boundary, so we don't want to be too lenient)
      for (int i = 0; i < 8; i++) {
        if (iso.get() != 0x00) {
          good = false;
          
          // this occurs only on map 0x1F2D,
          // and the end is still correctly found
//          if (flags & flag_isSmall) {
//            std::cerr << "small map with bad detection heuristic" << std::endl;
//          }
          
          break;
        }
        
        iso.seekoff(-2);
      }
      
      if (good) break;
      
      ++sectorCount;
    }
    
//    std::cerr << std::hex << sectorNum << " " << sectorCount << std::endl;
    
    iso.seek(mapBasePos);
    ifs.writeFrom(iso, (sectorCount * TenmaMsgConsts::sectorSize));
  }
  
//  ifs.writeFrom(iso, TenmaMsgConsts::mapMsgBlockSize);
  
  ifs.seek(0);
  if (textStartPtr != 0x0000) {
    ifs.seek(textStartPtr - 0xC000);
    if ((mapNamePtr != 0) && (mapNamePtr < textStartPtr)) {
//      std::cerr << "map name < text start" << std::endl;
      ifs.seek(0);
    }
  }
  else {
//    std::cerr << "no text start ptr?" << std::endl;
  }
  
  std::string mapLabel = std::string("map ")
    + TStringConversion::intToString(sectorNum,
        TStringConversion::baseHex);
  if (flags & flag_isSmall) {
    mapLabel += " (small, not used?)";
  }
  
  addComment(mapLabel);
  addSetRegion(std::string("map_")
    + TStringConversion::intToString(sectorNum,
        TStringConversion::baseHex));
  addSetRegionProp("mapStartSector",
    TStringConversion::intToString(sectorNum,
        TStringConversion::baseHex));
  
  int searchStartPos = ifs.tell();
  
  ifs.seek(ifs.size() - 1);
  while (true) {
    if (ifs.get() != 0x00) break;
    ifs.seekoff(-2);
  }
  
  int endNullCount = ifs.size() - ifs.tell();
//  std::cerr << std::hex << endNullCount << std::endl;
  int paddingToFree = endNullCount - 32;
  if (paddingToFree > 0) {
    addFreeSpace(ifs.size() - paddingToFree, paddingToFree);
  }
  
  // HACK: every map has enough space without hacks except this one
  if (sectorNum == 0xE5CF) {
    addFreeSpace(0x800, 0x800);
  }
  
  ifs.seek(searchStartPos);
  
  addMap(ifs, mapIndex, table, sectorNum, flags);
}

void TenmaScriptGenStream::addRawUnterminatedString(BlackT::TStream& ifs,
                         int length,
                         const BlackT::TThingyTable& table,
                         std::string name) {
  int startPos = ifs.tell();
  int endPos = startPos + length;
  std::string result;
  while (ifs.tell() < endPos) {
    TThingyTable::MatchResult matchResult = table.matchId(ifs);
    if (matchResult.id == -1) {
      throw TGenericException(T_SRCANDLINE,
                                  "TenmaMsgDismEntry::addRawUnterminatedString()",
                                  "Could not match symbol");
    }
    result += table.getEntry(matchResult.id);
  }
  
  if (name.empty()) {
    name = std::string("rawstr-")
          + TStringConversion::intToString(startPos,
              TStringConversion::baseHex);
  }
  
  std::map<std::string, std::string> propertyMap;
  propertyMap["noTerminator"] = "1";
  
  addString(result,
          startPos,
          length,
          name,
          propertyMap);
}

void TenmaScriptGenStream::addGenericString(BlackT::TStream& ifs,
                         const BlackT::TThingyTable& table,
                         std::string name,
                         bool allowAscii) {
  TenmaMsgDism dism;
  try {
    dism.dism(ifs,
              TenmaMsgDism::dismMode_raw,
              TenmaMsgDism::msgTypeMode_noCheck,
              allowAscii);
  }
  catch (TenmaMsgDismException& e) {
    std::cerr << "exception at " << std::hex << ifs.tell()
      << ": " << e.problem() << std::endl;
    throw e;
  }
  
  if (name.empty()) {
    name = std::string("str-")
          + TStringConversion::intToString(dism.msgContentStartOffset,
              TStringConversion::baseHex);
  }
  
  std::map<std::string, std::string> propertyMap;
  if (dism.terminatedByPrompt) {
    propertyMap["terminatedByPrompt"] = "1";
  }
  else if (dism.terminatedByF9) {
    propertyMap["terminatedByF9"] = "1";
  }
  
  addString(dism.getStringForm(table),
          dism.msgContentStartOffset,
          dism.msgEndOffset - dism.msgContentStartOffset,
          name,
          propertyMap);
}

void TenmaScriptGenStream::addScriptString(BlackT::TStream& ifs,
                 const BlackT::TThingyTable& table,
                 std::string name,
                 TenmaMsgDism::MsgTypeMode mode) {
  TenmaMsgDism dism;
  try {
    dism.dism(ifs,
              TenmaMsgDism::dismMode_full,
              mode);
  }
  catch (TenmaMsgDismException& e) {
    std::cerr << "exception at " << std::hex << ifs.tell()
      << ": " << e.problem() << std::endl;
    throw e;
  }
  
  if (name.empty()) {
    name = std::string("str-")
          + TStringConversion::intToString(dism.msgContentStartOffset,
              TStringConversion::baseHex);
  }
  
  std::map<std::string, std::string> propertyMap;
  if (dism.terminatedByPrompt) {
    propertyMap["terminatedByPrompt"] = "1";
  }
  else if (dism.terminatedByF9) {
    propertyMap["terminatedByF9"] = "1";
  }
  
  TenmaSubStringCollection subStrings
    = dism.getSplitStringForm(table);
  addSplitString(subStrings,
          dism.msgContentStartOffset,
          dism.msgEndOffset - dism.msgContentStartOffset,
          name,
          propertyMap);
}

void TenmaScriptGenStream::addMenu(BlackT::TStream& ifs,
                                   const BlackT::TThingyTable& table,
                                   std::string name,
                                   TenmaMsgDism::MenuTypeMode mode) {
  TenmaMsgDism dism;
  try {
    dism.dismMenu(ifs, mode);
  }
  catch (TenmaMsgDismException& e) {
    std::cerr << "exception at " << std::hex << ifs.tell()
      << ": " << e.problem() << std::endl;
    throw e;
  }
  
  if (name.empty()) {
    name = std::string("str-")
          + TStringConversion::intToString(dism.msgContentStartOffset,
              TStringConversion::baseHex);
  }
  
//  TenmaSubStringCollection subStrings
//    = dism.getSplitStringForm(table);
//  addSplitString(subStrings,
//          dism.msgContentStartOffset,
//          dism.msgEndOffset - dism.msgContentStartOffset,
//          name);
  
  
  std::map<std::string, std::string> propertyMap;
  if (mode == TenmaMsgDism::menuTypeMode_params) {
    propertyMap["noTerminator"] = "1";
  }
  
  addString(dism.getStringForm(table, true),
          dism.msgContentStartOffset,
          dism.msgEndOffset - dism.msgContentStartOffset,
          name,
          propertyMap);
}

void TenmaScriptGenStream::addString(
    std::string str, int offset, int size, std::string id,
    std::map<std::string, std::string> propertyMap) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_string;
  result.content = str;
  result.offset = offset;
  result.size = size;
  result.idOverride = id;
  result.propertyMap = propertyMap;
  entries.push_back(result);
}

void TenmaScriptGenStream::addSplitString(
    TenmaSubStringCollection& subStrings,
    int offset, int size, std::string id,
    std::map<std::string, std::string> propertyMap) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_string;
//  result.content = str;
  result.subStrings = subStrings;
  result.offset = offset;
  result.size = size;
  result.idOverride = id;
  result.propertyMap = propertyMap;
  entries.push_back(result);
}

void TenmaScriptGenStream::addMap(TStream& ifs,
            TenmaMapIndex& mapIndex,
            const TThingyTable& table,
            int mapId,
            int flags) {
  while (!ifs.eof()) {
    int basePos = ifs.tell();
    
    TenmaMsgDism dism;
    try {
      dism.dism(ifs,
                TenmaMsgDism::dismMode_full,
                TenmaMsgDism::msgTypeMode_check);
      
      // throw away anything without string content
      if (dism.hasStringContent()) {
/*        addString(dism.getStringForm(table),
          dism.msgContentStartOffset,
          dism.msgEndOffset - dism.msgContentStartOffset,
          std::string("map-")
          + TStringConversion::intToString(mapId,
              TStringConversion::baseHex)
          + "-"
          + TStringConversion::intToString(dism.msgContentStartOffset,
              TStringConversion::baseHex));*/
        
        // HACK: misdetections
        if (((mapId == 0x30C0) && (dism.msgContentStartOffset == 0x2E8))) {
          ifs.seek(basePos + 1);
          continue;
        }
    
        // HACK: there is a persistent misdetection issue involving
        // some data with the repeated byte "82" (SJIS 8282 = the letter "b").
        // detect and avoid such data.
        int endPos = ifs.tell();
        if (dism.msgEndOffset - dism.msgContentStartOffset >= 6) {
          ifs.seek(dism.msgContentStartOffset);
          bool bad = true;
          for (int i = 0; i < 6; i++) {
            if (ifs.readu8() != 0x82) {
              bad = false;
              break;
            }
          }
          
          if (bad) {
            ifs.seek(basePos + 1);
            continue;
          }
          
          ifs.seek(endPos);
        }
        
        std::map<std::string, std::string> propertyMap;
        if (dism.terminatedByPrompt) {
          propertyMap["terminatedByPrompt"] = "1";
        }
        else if (dism.terminatedByF9) {
          propertyMap["terminatedByF9"] = "1";
        }
        
        TenmaSubStringCollection subStrings
          = dism.getSplitStringForm(table);
        addSplitString(subStrings,
          dism.msgContentStartOffset,
          dism.msgEndOffset - dism.msgContentStartOffset,
          std::string("map-")
          + TStringConversion::intToString(mapId,
              TStringConversion::baseHex)
          + "-"
          + TStringConversion::intToString(dism.msgContentStartOffset,
              TStringConversion::baseHex),
          propertyMap);
        
        continue;
      }
    }
    catch (TenmaMsgDismException& e) {
//      std::cout << std::hex << basePos << ": " << e.problem() << std::endl;
    }
    
    ifs.seek(basePos + 1);
  }
}

void TenmaScriptGenStream::addPlaceholderString(std::string id,
    bool noTerminator) {
//  addString("", 0, 0, id);
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_string;
  result.content = "";
  result.offset = 0;
  result.size = 0;
  result.idOverride = id;

  std::map<std::string, std::string> propertyMap;
  if (noTerminator) {
    propertyMap["noTerminator"] = "1";
  }
  result.propertyMap = propertyMap;
  
  result.mayNotExist = true;
  entries.push_back(result);
}

void TenmaScriptGenStream::addPlaceholderStringSet(std::string id, int count,
    bool noTerminator) {
  for (int i = 0; i < count; i++) {
    addPlaceholderString(id + "_" + TStringConversion::intToString(i),
      noTerminator);
  }
}

void TenmaScriptGenStream::addComment(std::string comment) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_comment;
  result.content = comment;
  entries.push_back(result);
}

void TenmaScriptGenStream::addSmallComment(std::string comment) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_smallComment;
  result.content = comment;
  entries.push_back(result);
}

void TenmaScriptGenStream::addSetRegion(std::string name) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_setRegion;
  result.content = name;
  entries.push_back(result);
}

void TenmaScriptGenStream::addGenericLine(std::string content) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_genericLine;
  result.content = content;
  entries.push_back(result);
}

void TenmaScriptGenStream::addSetRegionProp(
    std::string name, std::string value) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_setRegionProp;
  result.propName = name;
  result.propValue = value;
  entries.push_back(result);
}

void TenmaScriptGenStream::addSetStringProp(
    std::string name, std::string value) {
  TenmaScriptGenStreamEntry result;
  result.type = TenmaScriptGenStreamEntry::type_setStringProp;
  result.propName = name;
  result.propValue = value;
  entries.push_back(result);
}

void TenmaScriptGenStream::addSetSize(int w, int h) {
  std::string str = std::string("#SETSIZE(")
    + TStringConversion::intToString(w)
    + ", "
    + TStringConversion::intToString(h)
    + ")";
  addGenericLine(str);
}

void TenmaScriptGenStream::addFreeSpace(int offset, int size) {
  std::string str = std::string("#ADDFREESPACE(")
    + TStringConversion::intToString(offset,
        TStringConversion::baseHex)
    + ", "
    + TStringConversion::intToString(size,
        TStringConversion::baseHex)
    + ")";
  addGenericLine(str);
}


}
