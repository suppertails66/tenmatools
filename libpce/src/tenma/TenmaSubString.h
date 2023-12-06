#ifndef TENMASUBSTRING_H
#define TENMASUBSTRING_H


#include "util/TStream.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>
#include <map>

namespace Pce {


class TenmaSubString {
public:
  TenmaSubString();
  
  std::string content;
  std::string prefix;
  std::string translationPlaceholder;
  bool visible;
  
protected:
  
};

typedef std::vector<TenmaSubString> TenmaSubStringCollection;


}


#endif
