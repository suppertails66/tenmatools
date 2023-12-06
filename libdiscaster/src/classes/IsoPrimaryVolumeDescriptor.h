#ifndef DISCASTERISOPRIMARYVOLUMEDESCRIPTOR_H
#define DISCASTERISOPRIMARYVOLUMEDESCRIPTOR_H


#include "discaster/Object.h"
#include "discaster/CdConsts.h"
#include "discaster/ParseNode.h"
#include "util/TStream.h"
#include <string>

namespace Discaster {

  
  class IsoPrimaryVolumeDescriptor : public BlobObject {
  public:
    IsoPrimaryVolumeDescriptor();
  
    static void write(
        const Object* env, const Object* self, BlackT::TStream& dst,
        CdFormatIds::CdFormatId cdFormat);
  };


}


#endif
