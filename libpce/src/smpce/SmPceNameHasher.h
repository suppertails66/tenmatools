#ifndef SMPCENAMEHASHER_H
#define SMPCENAMEHASHER_H


//#include "util/TStream.h"
#include <string>
#include <iostream>

namespace Pce {


class SmPceNameHasher {
public:
  
  static std::string hash(const std::string& name);
  
protected:
  
};


}


#endif
