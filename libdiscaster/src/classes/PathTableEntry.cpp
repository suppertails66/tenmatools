#include "classes/PathTableEntry.h"
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
   * Writes N bytes to dst.
   * Content will be filled with str and padded to the required
   * size with bytes of the given value.
   * An error occurs if the input string exceeds the target length.
   *
   * @arg str Source string.
   * @arg dst Destination stream.
   * @arg size Number of output bytes.
   * @arg padval Value to pad with.
   */
/*  static void TStringConversion::writePaddedString(std::string str, BlackT::TStream& dst,
                             int size, BlackT::TByte padval = 0x00) {
    if (str.size() > size) {
      throw TGenericException(T_SRCANDLINE,
                              "writeZeroFilledString()",
                              std::string("String \"")
                                + str
                                + "\""
                                + " too long to write: limit is "
                                + TStringConversion::toString(size));
    }
    
    dst.write(str.c_str(), str.size());
    
    int remaining = size - str.size();
    for (int i = 0; i < remaining; i++) {
      dst.put(padval);
    }
  } */
  
  /**
   * Writes a PathTableEntry.
   */
  void PathTableEntry::write(
        const Object* env, const Object* self, BlackT::TStream& dst,
        BlackT::EndiannessTypes::EndiannessType endianness) {
    std::string name = self->getMemberString("name");
    
    int numNamePaddingBytes = ((name.size() & 0x01) != 0) ? 1 : 0;
    
    dst.writeu8(name.size());
    dst.writeu8(self->getMemberInt("extendedAttributeRecordLength"));
    dst.writeInt(self->getMemberInt("logicalBlockNumber"),
                 4, endianness, SignednessTypes::nosign);
    dst.writeInt(self->getMemberInt("parentDirectoryNumber"),
                 2, endianness, SignednessTypes::nosign);
    dst.write(name.c_str(), name.size());
    for (int i = 0; i < numNamePaddingBytes; i++) dst.put(0x00);
  }
  
  PathTableEntry::PathTableEntry()
    : BlobObject() {
    //============================
    // Set up functions
    //============================
    
//    addFunction("importDirectoryListing", importDirectoryListing);
    
    //============================
    // Set up members
    //============================
    
    // TODO: extended attribute records not supported
    setMember("extendedAttributeRecordLength", 0);
    setMember("logicalBlockNumber", 0);
    setMember("parentDirectoryNumber", 0);
    setMember("name", "");
    
    setMember("fullPath", "");
    
  }


}
