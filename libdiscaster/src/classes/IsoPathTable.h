#ifndef DISCASTERISOPATHTABLE_H
#define DISCASTERISOPATHTABLE_H


#include "discaster/Object.h"
#include "discaster/CdConsts.h"
#include "discaster/ParseNode.h"
#include "classes/FileListing.h"
#include "util/TStream.h"
#include "util/EndiannessType.h"
#include <string>

namespace Discaster {

  
  class IsoPathTable : public BlobObject {
  public:
    IsoPathTable();
  
    static void fromFileListing(
        Object* env, Object* self, Object* fileListingObj);
  
    static void write(
        const Object* env, const Object* self, BlackT::TStream& dst,
        BlackT::EndiannessTypes::EndiannessType endianness);
  };


}


#endif
