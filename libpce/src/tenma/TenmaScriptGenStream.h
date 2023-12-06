#ifndef TENMASCRIPTGENSTREAM_H
#define TENMASCRIPTGENSTREAM_H


#include "tenma/TenmaTranslationSheet.h"
#include "tenma/TenmaMapIndex.h"
#include "tenma/TenmaSubString.h"
#include "tenma/TenmaMsgDism.h"
#include "util/TStream.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace Pce {


class TenmaScriptGenStreamEntry {
public:
  TenmaScriptGenStreamEntry();
  
  enum Type {
    type_none,
    type_comment,
    type_smallComment,
    type_string,
    type_setMap,
    type_setRegion,
    type_marker,
    type_genericLine,
    type_setRegionProp,
    type_setStringProp
  };
  
  Type type;
  
  std::string content;
  // HACK: if this is not empty, its strings are dumped in split mode
  // and content string is ignored
  TenmaSubStringCollection subStrings;
  std::map<std::string, std::string> propertyMap;
  
  std::string translationPlaceholder;
  
  int offset;
  int size;
  
  int mapSectorNum;
//  int regionId;
  bool mayNotExist;
  
  std::string idOverride;
  
  std::string propName;
  std::string propValue;
  
protected:
  
};

typedef std::vector<TenmaScriptGenStreamEntry>
    TenmaScriptGenStreamEntryCollection;

class TenmaScriptGenStream {
public:
  enum MapDumpFlags {
    flag_null            = 0,
    flag_isSmall         = (1 << 0),
    flag_hasLargeSpacing = (1 << 1)
  };  
  
  TenmaScriptGenStream();
  
  TenmaScriptGenStreamEntryCollection entries;
  
  void exportToSheet(
      TenmaTranslationSheet& dst,
      std::ostream& ofs,
      std::string idPrefix) const;
  
  void addMapFromIsoSector(BlackT::TStream& iso, int sectorNum,
                           TenmaMapIndex& mapIndex,
                           const BlackT::TThingyTable& table,
                           int flags = 0);
  
  void addRawUnterminatedString(BlackT::TStream& ifs,
                         int length,
                         const BlackT::TThingyTable& table,
                         std::string name = "");
  void addGenericString(BlackT::TStream& ifs,
                         const BlackT::TThingyTable& table,
                         std::string name = "",
                         bool allowAscii = false);
  void addScriptString(BlackT::TStream& ifs,
                       const BlackT::TThingyTable& table,
                       std::string name = "",
                       TenmaMsgDism::MsgTypeMode mode
                         = TenmaMsgDism::msgTypeMode_check);
  void addMenu(BlackT::TStream& ifs,
               const BlackT::TThingyTable& table,
               std::string name = "",
               TenmaMsgDism::MenuTypeMode mode
                 = TenmaMsgDism::menuTypeMode_noParams);
  
  void addString(std::string str, int offset, int size, std::string id,
    std::map<std::string, std::string> propertyMap
      = std::map<std::string, std::string>());
  void addSplitString(TenmaSubStringCollection& subStrings,
    int offset, int size, std::string id,
    std::map<std::string, std::string> propertyMap
      = std::map<std::string, std::string>());
  void addMap(BlackT::TStream& ifs,
              TenmaMapIndex& mapIndex,
              const BlackT::TThingyTable& table,
              int mapId,
              int flags = 0);
  
  void addPlaceholderString(std::string id, bool noTerminator = false);
  void addPlaceholderStringSet(std::string id, int count,
    bool noTerminator = false);
  
  void addComment(std::string comment);
  void addSmallComment(std::string comment);
  void addSetRegion(std::string name);
  void addGenericLine(std::string content);
  void addSetRegionProp(std::string name, std::string value);
  void addSetStringProp(std::string name, std::string value);
  void addSetSize(int w, int h);
  void addFreeSpace(int offset, int size);
protected:
  
};


}


#endif
