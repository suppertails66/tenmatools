#ifndef TENMAMSGDISM_H
#define TENMAMSGDISM_H


#include "tenma/TenmaSubString.h"
#include "util/TStream.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>
#include <map>

namespace Pce {


class TenmaMsgDismEntry {
public:
//  TenmaMsgDismEntry();
  
  enum Type {
    type_none,
    type_string,
    type_op,
    type_null,
    type_literalString
  };
  
  Type type;
  std::string content;
  int opNum;
  
  std::string getStringForm(const BlackT::TThingyTable& table) const;
  
protected:
  
};

typedef std::vector<TenmaMsgDismEntry> TenmaMsgDismEntryCollection;

class TenmaMsgDism {
public:
  TenmaMsgDism();
  
  enum DismMode {
    dismMode_full,
    dismMode_raw
  };
  
  enum MsgTypeMode {
    msgTypeMode_check,
    msgTypeMode_noCheck
  };
  
  enum MenuTypeMode {
    menuTypeMode_params,
    menuTypeMode_noParams
  };
  
//  void save(BlackT::TStream& ofs) const;
//  void load(BlackT::TStream& ifs);
  
  void dism(BlackT::TStream& ifs,
            DismMode dismMode,
            MsgTypeMode msgTypeMode,
            bool allowAscii = false);
  void dismMenu(BlackT::TStream& ifs,
                MenuTypeMode mode);
  
  bool hasStringContent() const;
  void stripInitialNonStringContent();
  
  std::string getStringForm(const BlackT::TThingyTable& table,
                            bool noAutoLinebreak = false) const;
  
  TenmaSubStringCollection getSplitStringForm(
    const BlackT::TThingyTable& table) const;
  
  int msgType;
  int msgTypeArg;
  int msgStartOffset;
  int msgContentStartOffset;
  int msgFirstStringStartOffset;
  int msgEndOffset;
  bool terminatedByPrompt;
  bool terminatedByF9;
  TenmaMsgDismEntryCollection entries;
  
protected:
  bool tryStringRead(BlackT::TStream& ifs, std::string& dst,
                     bool allowNull = false,
                     bool allowAscii = false,
                     bool noHighAscii = false);
  
  void generateNextSubStrings(
    const TenmaMsgDismEntryCollection& input,
    TenmaMsgDismEntryCollection::const_iterator& it,
    const BlackT::TThingyTable& table,
    TenmaSubStringCollection& dst,
    int& lineNum) const;
  static void outputLinebreaks(TenmaSubString& dst,
    const BlackT::TThingyTable& table, int count);
  static std::string getHexByte(BlackT::TStream& ifs);
};


}


#endif
