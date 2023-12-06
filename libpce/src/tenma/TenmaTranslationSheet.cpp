#include "tenma/TenmaTranslationSheet.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <vector>
#include <map>
#include <cctype>
#include <iostream>
#include <fstream>

using namespace BlackT;

namespace Pce {


void TenmaTranslationSheet::addStringEntry(std::string id, std::string content,
                    std::string prefix, std::string suffix,
                    std::string translationPlaceholder) {
  TenmaTranslationSheetEntry entry;
  entry.type = TenmaTranslationSheetEntry::type_string;
  entry.stringId = id;
  entry.stringContent = content;
  entry.stringPrefix = prefix;
  entry.stringSuffix = suffix;
  entry.translationPlaceholder = translationPlaceholder;
  
  entries.push_back(entry);
  stringEntryMap[id] = entry;
}

void TenmaTranslationSheet::addCommentEntry(std::string comment) {
  TenmaTranslationSheetEntry entry;
  entry.type = TenmaTranslationSheetEntry::type_comment;
  entry.stringContent
    = std::string("//==================================\n")
      + "// " + comment
      + std::string("\n//==================================");
  
  entries.push_back(entry);
}

void TenmaTranslationSheet::addSmallCommentEntry(std::string comment) {
  TenmaTranslationSheetEntry entry;
  entry.type = TenmaTranslationSheetEntry::type_comment;
  entry.stringContent
    = std::string("//=====\n")
      + "// " + comment
      + std::string("\n//=====");
  
  entries.push_back(entry);
}

void TenmaTranslationSheet::addMarkerEntry(std::string content) {
  TenmaTranslationSheetEntry entry;
  entry.type = TenmaTranslationSheetEntry::type_marker;
  entry.stringId = content;
  entries.push_back(entry);
}

bool TenmaTranslationSheet::hasStringEntryForId(std::string id) const {
  return (stringEntryMap.find(id) != stringEntryMap.end());
}

const TenmaTranslationSheetEntry& TenmaTranslationSheet
    ::getStringEntryById(std::string id) const {
  return stringEntryMap.at(id);
}

void TenmaTranslationSheet::importCsv(std::string filename) {
  TCsv csv;
  {
    TBufStream ifs;
    ifs.open(filename.c_str());
//    csv.readSjis(ifs);
    csv.readUtf8(ifs);
  }
  
//    std::cerr << "here1" << std::endl;
  for (int j = 0; j < csv.numRows(); j++) {
//    std::cerr << j << std::endl;
    std::string type = csv.cell(0, j);
    if (type.compare("string") == 0) {
      std::string id = csv.cell(1, j);
      std::string prefix = csv.cell(2, j);
      std::string suffix = csv.cell(3, j);
      std::string content = csv.cell(5, j);
      addStringEntry(id, content, prefix, suffix);
    }
  }
//    std::cerr << "here2" << std::endl;
}

void TenmaTranslationSheet::exportCsv(std::string filename,
                      TenmaTranslationSheet::LinkData* linkData) const {
  LinkData newLinkData;
  if (linkData == NULL) {
    // do "dummy" export to get link data
    exportCsv(filename, &newLinkData);
    linkData = &newLinkData;
  }
  
  std::ofstream ofs(filename.c_str(), std::ios_base::binary);

  // map of string content to first cell containing that content,
  // for generating cell equates for duplicate strings
  std::map<std::string, DupeResult> stringMap;
  
  std::map<std::string, std::string> boxMap;
  
  int rowNum = 0;
  bool inDictSection = false;
  for (TenmaTranslationSheetEntryCollection::const_iterator it
        = entries.cbegin();
       it != entries.cend();
       ++it) {
    const TenmaTranslationSheetEntry& entry = *it;
    switch (entry.type) {
    case TenmaTranslationSheetEntry::type_string:
    {
      // col A = type
      ofs << "\"string\"";
      
      // col B = id
      ofs << ",";
      ofs << "\"" << entry.stringId << "\"";
      
      // col C = prefix
      ofs << ",";
      ofs << "\"" << entry.stringPrefix << "\"";
      
      // col D = suffix
      ofs << ",";
      ofs << "\"" << entry.stringSuffix << "\"";
      
      // col E = content
      ofs << ",";
      std::map<std::string, DupeResult>::iterator findIt
        = stringMap.find(entry.stringContent);
      // to simplify clutter a bit: pretend null strings are not repeats
      if (findIt != stringMap.end()) {
        if (entry.stringContent.empty()) findIt = stringMap.end();
      }
      
      if (findIt != stringMap.end()) {
//        ofs << "\"=T(" << findIt->second << ")\"";
        ofs << "\"" << entry.stringContent << "\"";
      }
      else {
//        ofs << "\"" << entry.stringContent << "\"";
//        stringMap[entry.stringContent]
//          = TCsv::coordinatePosToCellId(translationContentColNum, rowNum);
        
        // if not linked, produce link data
        if (!linkData->linked) {
          ofs << "\"" << entry.stringContent << "\"";
          
          // add to dictionary if enabled
          if (inDictSection && (!entry.stringContent.empty())) {
            DictEntry dictEntry;
            dictEntry.content = entry.stringContent;
            dictEntry.rowNum = rowNum;
//            std::cerr << dictEntry.content << " " << dictEntry.rowNum << std::endl;
            linkData->dictEntries[dictEntry.content] = dictEntry;
          }
        }
        // if linked, check for dictionary substitution
        else {
          // of course, don't link dictionary entries to themselves
          if (inDictSection) {
            ofs << "\"" << entry.stringContent << "\"";
          }
          else {
//            ofs << "\"" << getDictionariedString(*linkData, entry.stringContent)
//                << "\"";
            ofs << "\"" << entry.stringContent << "\"";
          }
        }
        
        DupeResult res;
        res.stringId = entry.stringId;
        res.rowNum
          = TCsv::coordinatePosToCellId(translationContentColNum, rowNum);
        stringMap[entry.stringContent] = res;
      }
      
      // col F = translation placeholder
      ofs << ",";
      if (findIt != stringMap.end()) {
        ofs << "\"=T(" << findIt->second.rowNum << ")\"";
      }
      else {
//        ofs << "\"\"";

        ofs << "\""
          << entry.translationPlaceholder
          << "\"";
        
        // zenki says this the original text is more useful
        // than the placeholders i went to so much trouble to generate :(
//        ofs << "\""
//          << entry.stringContent
////          << getQuoteSubbedString(entry.stringContent)
//          << "\"";
      }
      
      // col G = comment
      ofs << ",";
      if (findIt != stringMap.end()) {
//        ofs << "\"DUPE: " << findIt->second << "\"";
//        ofs << "\"DUPE: " << findIt->second.stringId << "\"";
//        ofs << "\"=\"\"DUPE: \"\"&ROW(" << findIt->second.rowNum << ")\"";
//        ofs << "\"NOTE: duplicate\"";
        // this horrible mess is the only way i could find to print an
        // automatically-updating cell reference that works in openoffice...
        ofs << "\"=\"\"DUPE: \"\""
          << "&ADDRESS("
            << "ROW(" << findIt->second.rowNum << "), "
            << "COLUMN(" << findIt->second.rowNum << "), "
            << "4"
          << ")"
          << "\"";
      }
      else {
/*        // box duplication check
        std::map<std::string, std::string>::iterator boxFindIt
          = findBoxDuplicate(boxMap, entry.stringContent);
        if (boxFindIt != boxMap.end()) {
          ofs << "\"PARTIAL DUPE: " << boxFindIt->second << "\"";
        }
        addBoxDuplicates(boxMap, entry.stringContent,
          TCsv::coordinatePosToCellId(translationContentColNum, rowNum)); */
      }
      
      // col H = filled check
      ofs << ",";
      if ((findIt == stringMap.end())
          && !entry.stringContent.empty()) {
        ofs << "\"=IF(F" << rowNum+1 << "=I" << rowNum+1
          << ", \"\"UNTRANSLATED\"\", \"\"\"\")\"";
      }
      else {
        
      }
      
      // col I = filled check value
      ofs << ",";
      if (findIt == stringMap.end()) {
        ofs << "\""
          << entry.translationPlaceholder
          << "\"";
//        ofs << "\""
//          << entry.stringContent
////          << getQuoteSubbedString(entry.stringContent)
//          << "\"";
      }
      
      ofs << std::endl;
      ++rowNum;
    }
      break;
    case TenmaTranslationSheetEntry::type_comment:
    {
      // col A = type (blank to ignore)
      ofs << "\"\"";
      
      // col B
      ofs << ",";
      ofs << "\"\"";
      
      // col C
      ofs << ",";
      ofs << "\"\"";
      
      // col D
      ofs << ",";
      ofs << "\"\"";
      
      // col E
      ofs << ",";
      ofs << "\"" << entry.stringContent << "\"";
      
      // col F
      ofs << ",";
      ofs << "\"\"";
      
      // col G
      ofs << ",";
      ofs << "\"\"";
      
      // col H
      ofs << ",";
      ofs << "\"\"";
      
      // col I
      ofs << ",";
      ofs << "\"\"";
      
      ofs << std::endl;
      ++rowNum;
    }
      break;
    case TenmaTranslationSheetEntry::type_marker:
    {
      if (entry.stringId.compare("dict_section_start") == 0) {
        inDictSection = true;
      }
      else if (entry.stringId.compare("dict_section_end") == 0) {
        inDictSection = false;
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "TenmaTranslationSheet::exportCsv()",
                                "unknown marker: "
                                  + entry.stringId);
      }
    }
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "TenmaTranslationSheet::exportCsv()",
                              "illegal type");
      break;
    }
  }
  
  if (!linkData->linked) {
    linkData->linked = true;
  }
}

int TenmaTranslationSheet::numEntries() const {
  return entries.size();
}

void TenmaTranslationSheet::addBoxDuplicates(
    std::map<std::string, std::string>& boxMap,
    std::string content,
    std::string cell) {
  std::vector<std::string> boxes = splitByBoxes(content);
  for (std::vector<std::string>::iterator it = boxes.begin();
       it != boxes.end();
       ++it) {
    std::map<std::string, std::string>::iterator findIt
      = boxMap.find(*it);
    if (findIt == boxMap.end()) {
      boxMap[*it] = cell;
    }
  }
}

std::map<std::string, std::string>::iterator TenmaTranslationSheet
  ::findBoxDuplicate(
    std::map<std::string, std::string>& boxMap,
    std::string content) {
  std::vector<std::string> boxes = splitByBoxes(content);
  
  for (std::vector<std::string>::iterator it = boxes.begin();
       it != boxes.end();
       ++it) {
    std::map<std::string, std::string>::iterator findIt
      = boxMap.find(*it);
    if (findIt != boxMap.end()) {
      return findIt;
    }
  }
  
  return boxMap.end();
}

std::vector<std::string> TenmaTranslationSheet
  ::splitByBoxes(std::string content) {
  std::vector<std::string> result;
  
  if (content.empty()) return result;
  
  int startPos = 0;
  for (int i = 0; i < content.size() - 1; i++) {
    if ((content[i] == '\\') && (content[i + 1] == 'p')) {
      result.push_back(content.substr(startPos, (i - startPos) + 2));
      startPos = i + 2;
    }
  }
  
  if (startPos < content.size()) {
    result.push_back(content.substr(startPos, content.size() - startPos));
  }
  
  return result;
}

std::string TenmaTranslationSheet::getDictionariedString(LinkData& linkData,
                                         std::string content) {
  std::string result;
  
  result += "=CONCATENATE(";
  
    std::vector<std::string> strings = splitByDictEntries(linkData, content);
    for (unsigned int i = 0; i < strings.size(); i++) {
      result += strings[i];
      if (i != strings.size() - 1) result += ", ";
    }
  
  result += ")";
  
  // my fucking god
  for (int i = 0; i < result.size(); i++) {
    if (result[i] == '\n') {
      result = result.substr(0, i)
        + "\"\", CHAR(10), \"\""
        + result.substr(i + 1, std::string::npos);
    }
  }
  
  return result;
}

std::vector<std::string> TenmaTranslationSheet::splitByDictEntries(
    LinkData& linkData, std::string content) {
  std::map<int, std::string> posToEntryContent;
  
//  std::cerr << content << std::endl;
  for (std::map<std::string, DictEntry>::iterator it
         = linkData.dictEntries.begin();
       it != linkData.dictEntries.end();
       ++it) {
    std::string dictContent = it->second.content;
    // does not handle matching subsequences correctly,
    // but input is such that it doesn't matter
    if (dictContent.size() > content.size()) continue;
    
    for (int i = 0; i < content.size() - dictContent.size(); ) {
//  std::cerr << i << std::endl;
      if (content.substr(i, dictContent.size()).compare(dictContent) == 0) {
        posToEntryContent[i] = dictContent;
        i += dictContent.size();
      }
      else {
        ++i;
      }
    }
  }
  
  std::vector<std::string> result;
  
  // NOTE: the nature of the input is such that overlapping sequences
  // should not be a problem
  int pos = 0;
  int endPos = content.size();
  for (std::map<int, std::string>::iterator it = posToEntryContent.begin();
       it != posToEntryContent.end();
       ++it) {
    // add content from current pos to entry start pos, if any
    int startSectionSize = it->first - pos;
    if (startSectionSize > 0) {
      result.push_back(std::string("\"\"")
        + content.substr(pos, startSectionSize)
        + "\"\"");
    }
    
    // add dictionary sequence
    DictEntry dictEntry = linkData.dictEntries[it->second];
    std::string origCellId = TCsv::coordinatePosToCellId(
      stringContentColNum, dictEntry.rowNum);
    std::string newCellId = TCsv::coordinatePosToCellId(
      translationContentColNum, dictEntry.rowNum);
    // use translation if it exists; otherwise, use original
    std::string newStr = std::string("IF(")
      + newCellId + "=\"\"\"\", "
      + origCellId
      + ", "
      + newCellId
      + ")";
    result.push_back(newStr);
    
    // update current pos to past end of dictionaried term
    pos = it->first + it->second.size();
  }
  
  // add final literal string, if any
  // (entire string if no dictionary sequences)
  if ((endPos - pos) != 0) {
//    result.push_back(content.substr(pos, endPos));
    result.push_back(std::string("\"\"")
      + content.substr(pos, endPos)
      + "\"\"");
  }
  
  return result;
}

std::string TenmaTranslationSheet::getQuoteSubbedString(std::string content) {
  std::vector<std::string> boxStrings = splitByBoxes(content);
  
  std::string result;
  
  for (std::vector<std::string>::iterator it = boxStrings.begin();
       it != boxStrings.end();
       ++it) {
    std::string str = *it;
    TBufStream ifs;
    int quotePos;
    
    // ignore if no trailing box break
    if (str.size() < 2) goto fail;
    if ((str[str.size() - 2] != '\\')
        || (str[str.size() - 1] != 'p')) goto fail;
    
    ifs.writeString(str);
    ifs.seek(0);
    
    // skip initial whitespace
    TParse::skipSpace(ifs);
    
    // skip nametag if it exists
    if (ifs.remaining() < 2) goto fail;
    if (ifs.readu16be() == sjisOpenAngleBracket) {
      while (true) {
        if (ifs.remaining() < 2) goto fail;
        if (ifs.readu16be() == sjisCloseAngleBracket) {
          // check for nametag linebreak
          if (ifs.remaining() < 2) goto fail;
          if (ifs.readu8() != '\\') goto fail;
          if (ifs.readu8() != 'n') goto fail;
          
          break;
        }
      }
    }
    else {
      ifs.seekoff(-2);
    }
    
    // skip whitespace
    TParse::skipSpace(ifs);
    if (ifs.remaining() < 2) goto fail;
    
    // is next symbol a quote?
    if (ifs.readu16be() != sjisOpenQuote) goto fail;
    
    // substitute
//    success:
      quotePos = ifs.tell() - 2;
      str = str.substr(0, quotePos)
            + "{"
            + str.substr(quotePos + 2, str.size() - (quotePos + 2) - 2)
            + "}\\p";
      result += str;
      continue;
    
    fail:
      result += str;
      continue;
      
//    if (str.size() < 2) goto done;
//    
//    done:
//      result += str;
  }
  
  return result;
}

TenmaTranslationSheet::LinkData::LinkData()
  : linked(false) { }


}
