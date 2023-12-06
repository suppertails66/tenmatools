#ifndef TENMATRANSLATIONSHEET_H
#define TENMATRANSLATIONSHEET_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TThingyTable.h"
#include "util/TCsv.h"
#include <map>
#include <string>
#include <vector>

namespace Pce {


class TenmaTranslationSheetEntry {
public:
  enum Type {
    type_none,
    type_string,
    type_comment,
    type_marker
  };
  
  Type type;
  
  std::string stringId;
  std::string stringContent;
  std::string stringPrefix;
  std::string stringSuffix;
  
  std::string translationPlaceholder;
  
protected:
};

typedef std::vector<TenmaTranslationSheetEntry>
  TenmaTranslationSheetEntryCollection;

class TenmaTranslationSheet {
protected:
  struct DictEntry {
    
    std::string content;
    int rowNum;
  };

  struct LinkData {
    LinkData();
  
    bool linked;
    
//    int items_baseRow;
//    int items_count;
//    std::vector<DictEntry> dictEntries;
    std::map<std::string, DictEntry> dictEntries;
  };
  
public:
  
  int numEntries() const;
  
  void addStringEntry(std::string id, std::string content,
                      std::string prefix = "", std::string suffix = "",
                      std::string translationPlaceholder = "");
  void addCommentEntry(std::string comment);
  void addSmallCommentEntry(std::string comment);
  void addMarkerEntry(std::string content);
  
  bool hasStringEntryForId(std::string id) const;
  const TenmaTranslationSheetEntry& getStringEntryById(std::string id) const;
  
  void importCsv(std::string filename);
  void exportCsv(std::string filename, LinkData* linkData = NULL) const;
  
protected:

  const static int stringContentColNum = 4;
  const static int translationContentColNum = 5;
  const static int sjisOpenAngleBracket = 0x8171;
  const static int sjisCloseAngleBracket = 0x8172;
  const static int sjisOpenQuote = 0x8175;
  const static int sjisCloseQuote = 0x8176;

  TenmaTranslationSheetEntryCollection entries;
  std::map<std::string, TenmaTranslationSheetEntry> stringEntryMap;
  
  static void addBoxDuplicates(
      std::map<std::string, std::string>& boxMap,
      std::string content,
      std::string cell);
  static std::map<std::string, std::string>::iterator findBoxDuplicate(
      std::map<std::string, std::string>& boxMap,
      std::string content);
  static std::vector<std::string> splitByBoxes(std::string content);
  
  static std::string getDictionariedString(LinkData& linkData,
                                           std::string content);
  static std::vector<std::string> splitByDictEntries(LinkData& linkData,
                                           std::string content);
  
  static std::string getQuoteSubbedString(std::string content);
  
  struct DupeResult {
    std::string stringId;
    std::string rowNum;
  };
};


}


#endif
