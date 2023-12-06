#include "yuna/YunaTranslationSheet.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include "exception/TException.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>

using namespace BlackT;

namespace Pce {


void YunaTranslationSheet::exportCsv(std::string filename) const {
  std::ofstream ofs;
  ofs.open(filename.c_str(), std::ios_base::binary);
  
  // map of unique string contents to cell ID
  std::map<std::string, std::string> entriesByContent;
  
  // new map to hold optimized content
  IdToStringMap entriesOptimized = entries;
  
  int rowNum = 0;
  for (IdToStringMap::iterator it = entriesOptimized.begin();
       it != entriesOptimized.end();
       ++it) {
    if (it->second.command == "String") {
      std::map<std::string, std::string>::iterator findIt
        = entriesByContent.find(it->second.original);
      if (findIt != entriesByContent.end()) {
        // replace with reference to first instance of string
        it->second.translation
          = std::string("=T(") + findIt->second + ")";
//        it->second.comment = std::string("DUPE: ") + findIt->second;
        it->second.comment = std::string("DUPE: ")
          + findIt->second.substr(1, std::string::npos);
      }
      else {
        // add new
        entriesByContent[it->second.original] = 
//          std::string("=T(")
//          + TCsv::coordinatePosToCellId(translationColNum, rowNum)
//          + ")";
          TCsv::coordinatePosToCellId(translationColNum, rowNum);
      }
    }
    
    it->second.exportCsv(ofs);
    
    ++rowNum;
  }
}

void YunaTranslationSheet::importCsv(std::string filename) {
  entries.clear();  
  
  TCsv csv;
  TIfstream ifs;
  ifs.open(filename.c_str(), std::ios_base::binary);
  csv.readUtf8(ifs);
  
  for (int j = 0; j < csv.numRows(); j++) {
    YunaTranslationString transStr;
    transStr.command = csv.cell(0, j);
    transStr.id = TStringConversion::stringToInt(csv.cell(1, j));
    transStr.sharedContentPre = csv.cell(2, j);
    transStr.sharedContentPost = csv.cell(3, j);
    transStr.original = csv.cell(4, j);
    transStr.translation = csv.cell(5, j);
    
    entries[transStr.id] = transStr;
  }
}

void YunaTranslationSheet::addEntry(const YunaTranslationString& entry) {
  YunaTranslationString newEntry = entry;
  int id = nextEntryId();
  newEntry.id = id;
  entries[id] = newEntry;
}

YunaTranslationString YunaTranslationSheet::getEntry(int id) {
  return entries.at(id);
}

int YunaTranslationSheet::nextEntryId() const {
  // this is +1 solely so that the assigned IDs will match
  // row numbers in the output CSV by default
  return entries.size() + 1;
}


}
