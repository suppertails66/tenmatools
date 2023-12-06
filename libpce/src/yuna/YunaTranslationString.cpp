#include "yuna/YunaTranslationString.h"
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


YunaTranslationString::YunaTranslationString()
  : command("String") { }
  
void YunaTranslationString::exportCsv(std::ostream& ofs) const {
  // command
  ofs << "\"" << command << "\"";
  ofs << ",";
  
  // id
  if (command.compare("String") == 0) {
    ofs << id;
  }
  ofs << ",";
  
  ofs << "\"" << sharedContentPre << "\"";
  ofs << ",";
  
  ofs << "\"" << sharedContentPost << "\"";
  ofs << ",";
  
  ofs << "\"" << original << "\"";
  ofs << ",";
  
  ofs << "\"" << translation << "\"";
  ofs << ",";
  
  ofs << "\"" << comment << "\"";
//  ofs << ",";
  
  ofs << std::endl;
}


}
