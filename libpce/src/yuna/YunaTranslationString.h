#ifndef YUNATRANSLATIONSTRING_H
#define YUNATRANSLATIONSTRING_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include "util/TCsv.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

namespace Pce {


class YunaTranslationString {
public:
  
  YunaTranslationString();
  
  std::string command;
  // arbitrarily assigned unique ID number within target context
  int id;
  // shared content before the part requiring translation
  // (e.g. color change codes)
  std::string sharedContentPre;
  // shared content after the part requiring translation
  // (e.g. "more" indicator arrow)
  std::string sharedContentPost;
  // original text (not including shared content)
  std::string original;
  // translated text (not including shared content)
  std::string translation;
  std::string comment;
  
  void exportCsv(std::ostream& ofs) const;
//  void importCsv(const BlackT::TCsv& src);
  
protected:
  
};


}


#endif
