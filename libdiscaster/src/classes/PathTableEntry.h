#ifndef DISCASTERPATHTABLEENTRY_H
#define DISCASTERPATHTABLEENTRY_H


#include "discaster/Object.h"
#include "discaster/CdConsts.h"
#include "discaster/ParseNode.h"
#include "util/TStream.h"
#include "util/EndiannessType.h"
#include <string>

namespace Discaster {

  
  class PathTableEntry : public BlobObject {
  public:
    PathTableEntry();
  
    static void write(
        const Object* env, const Object* self, BlackT::TStream& dst,
        BlackT::EndiannessTypes::EndiannessType endianness);
  };


}


#endif
