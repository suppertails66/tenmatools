#ifndef YUNATRANSLATIONSHEET_H
#define YUNATRANSLATIONSHEET_H


#include "yuna/YunaTranslationString.h"
#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include "util/TCsv.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Pce {


class YunaTranslationSheet {
public:
  typedef std::map<int, YunaTranslationString> IdToStringMap;
  
//  void exportCsv(BlackT::TCsv& dst);
  void exportCsv(std::string filename) const;
  void importCsv(std::string filename);
  
  void addEntry(const YunaTranslationString& entry);
  YunaTranslationString getEntry(int id);
  int nextEntryId() const;
  
protected:
  const static int originalColNum = 4;
  const static int translationColNum = 5;
  
//  typedef std::map<std::string, int> ContentToIdMap;
  
  IdToStringMap entries;
  
};


}


#endif
