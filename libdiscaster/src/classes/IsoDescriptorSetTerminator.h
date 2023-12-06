#ifndef DISCASTERISODESCRIPTORSETTERMINATOR_H
#define DISCASTERISODESCRIPTORSETTERMINATOR_H


#include "discaster/Object.h"
#include "discaster/CdConsts.h"
#include "discaster/ParseNode.h"
#include "util/TStream.h"
#include <string>

namespace Discaster {

  
  class IsoDescriptorSetTerminator : public BlobObject {
  public:
    IsoDescriptorSetTerminator();
  
    static void write(
        const Object* env, const Object* self, BlackT::TStream& dst);
  };


}


#endif
