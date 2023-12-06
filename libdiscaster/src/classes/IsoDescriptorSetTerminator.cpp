#include "classes/IsoDescriptorSetTerminator.h"
#include "classes/FileListing.h"
#include "discaster/CdConsts.h"
#include "discaster/CdBuilder.h"
#include "discaster/Config.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TFileManip.h"
#include "util/TFileInfo.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <fstream>

using namespace BlackT;

namespace Discaster {


  /**
   * Helper function to add a build command to the build command list.
   */
/*  static void addBuildCommand(Object* self, const Object& command) {
    if (self != NULL) {
     self->getMember("buildList").arrayValue().push_back(command);
   }
  } */

  /**
   * Imports a directory listing.
   * Scans a target directory for files and folders, and adds them to an
   * internal index of files. These imported files can then be referenced
   * with commands like addListedFile() in order to physically place them
   * onto the disc.
   * The import is recursive: all contents of all subdirectories in the target
   * directory will also be imported.
   *
   * arg0 inputFolder Folder on disk to import.
   * arg1 targetFolder TODO Optional: target directory on built disc.
   *                   Defaults to root directory.
   */
/*  static ParseNode* importDirectoryListing(
      Object* env, Object* self, ObjectArray args) {
    std::string inputFolder = args.at(0).stringValue();
    
    
    
    return NULL;
  } */
  
  /**
   * Writes a Primary Volume Descriptor.
   *
   * @arg dst TStream to write data to.
   */
  void IsoDescriptorSetTerminator::write(
      const Object* env, const Object* self, BlackT::TStream& dst) {
    dst.writeu8(self->getMemberInt("volumeDescriptorType"));
    std::string standardIdentifier
      = self->getMemberString("standardIdentifier");
    dst.write(standardIdentifier.c_str(), standardIdentifier.size());
    dst.writeu8(self->getMemberInt("volumeDescriptorVersion"));
    TStringConversion::writePaddedString(self->getMemberString("reserved7"), dst, 2041);
  }
  
  IsoDescriptorSetTerminator::IsoDescriptorSetTerminator()
    : BlobObject() {
    //============================
    // Set up functions
    //============================
    
//    addFunction("importDirectoryListing", importDirectoryListing);
    
    //============================
    // Set up members
    //============================
    
    setMember("volumeDescriptorType", CdConsts::descriptorSetTerminatorTypeId);
    setMember("standardIdentifier", "CD001");
    setMember("volumeDescriptorVersion", 1);
    setMember("reserved7", "");
  }


}
