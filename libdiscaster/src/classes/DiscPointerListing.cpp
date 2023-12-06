#include "classes/DiscPointerListing.h"
#include "discaster/CdConsts.h"
#include "discaster/CdBuilder.h"
#include "discaster/Config.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TFileInfo.h"
#include "util/TByte.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cctype>
#include <ctime>
#include <fstream>

using namespace BlackT;

namespace Discaster {

  
  DiscPointerListing::DiscPointerListing()
    : FileListing() {
    // override FileListing's identifier
    type_ = "DiscPointerListing";
    
    //============================
    // Set up members
    //============================
    
    setMember("startLabelName", "");
    setMember("endLabelName", "");
  }


}
