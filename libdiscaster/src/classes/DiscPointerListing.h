#ifndef DISCASTERDISCPOINTERLISTING_H
#define DISCASTERDISCPOINTERLISTING_H


#include "discaster/Object.h"
#include "discaster/CdConsts.h"
#include "discaster/ParseNode.h"
#include "classes/FileListing.h"
#include "util/TStream.h"
#include <string>

namespace Discaster {

  
  class DiscPointerListing : public FileListing {
  public:
    DiscPointerListing();
  };


}


#endif
