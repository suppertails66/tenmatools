#include "classes/IsoPathTable.h"
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
#include <vector>
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
  
  static void fromFileListingStep(
      Object* env, Object* self, Object* fileListingObj,
      ObjectArray& pathArray, std::map<std::string, int>& pathToDirIdMap,
      std::string path) {
    // level-order traversal (alphabetical by standard ISO9660 rule in each
    // level)
    
    // find directories
    Object& directoryMap
      = *FileListing::getDirectoryMap(env, fileListingObj);
    std::vector<Object*> directories;
    for (ObjectMemberMap::iterator it = directoryMap.members().begin();
         it != directoryMap.members().end();
         ++it) {
      Object& fileObj = it->second;
      if ((fileObj.getMemberInt("isDirectory") != 0)
          && (fileObj.getMemberInt("isSelfReference") == 0)
          && (fileObj.getMemberInt("isParentReference") == 0)) {
        directories.push_back(&fileObj);
      }
    }
    
    // sort directories
    std::sort(directories.begin(), directories.end(),
              FileListing::sortByIso9660NameP);
    
    // add to table
    for (std::vector<Object*>::iterator it = directories.begin();
         it != directories.end();
         ++it) {
      Object& fileObj = *(*it);
      
      std::string subname = fileObj.getMemberString("name");
      std::string subpath = path + "/" + subname;
      
      Object pathTableEntry = PathTableEntry();
      pathTableEntry.getMember("name").setStringValue(subname);
      pathTableEntry.getMember("fullPath").setStringValue(subpath);
      pathTableEntry.getMember("parentDirectoryNumber").setIntValue(
        pathToDirIdMap.at(path));
      
      pathArray.push_back(pathTableEntry);
      
      // note: directory numbers begin at 1, so this is "size()",
      // not "size() - 1"
      pathToDirIdMap[subpath] = pathArray.size();
    }
    
    // recursively add all subdirectories
//    for (ObjectMemberMap::const_iterator it = directoryMap.members().cbegin();
//         it != directoryMap.members().cend();
//         ++it) {
    for (std::vector<Object*>::iterator it = directories.begin();
         it != directories.end();
         ++it) {
      Object& fileObj = *(*it);
      
      std::string subname = fileObj.getMemberString("name");
      std::string subpath = path + "/" + subname;
      
      fromFileListingStep(env, self, &fileObj,
                          pathArray, pathToDirIdMap,
                          subpath);
    }
  }
  
  void IsoPathTable::fromFileListing(
      Object* env, Object* self, Object* fileListingObj) {
    self->setMember("pathArray", ArrayObject());
    ObjectArray& pathArray = self->getMember("pathArray").arrayValue();
    
    Object rootPathEntry = PathTableEntry();
    std::string rootNameString;
    rootNameString += CdConsts::directoryRecordSelfReferenceChar;
    rootPathEntry.getMember("name").setStringValue(rootNameString);
    // root is own parent
    rootPathEntry.getMember("parentDirectoryNumber").setIntValue(1);
    pathArray.push_back(rootPathEntry);
    
    // map of paths to directory ID number
    std::map<std::string, int> pathToDirIdMap;
    // root (always directory 1)
    pathToDirIdMap[""] = 1;
    
    fromFileListingStep(env, self, fileListingObj, pathArray, pathToDirIdMap,
                        "");
  }
  
  /**
   * Writes a Path Table.
   */
  void IsoPathTable::write(
        const Object* env, const Object* self, BlackT::TStream& dst,
        BlackT::EndiannessTypes::EndiannessType endianness) {
//    dst.writeu8(self->getMemberInt("volumeDescriptorType"));
    
    const ObjectArray& pathArray = self->getMember("pathArray").arrayValue();
    for (ObjectArray::const_iterator it = pathArray.cbegin();
         it != pathArray.cend();
         ++it) {
      PathTableEntry::write(env, &(*it), dst, endianness);
    }
  }
  
  IsoPathTable::IsoPathTable()
    : BlobObject() {
    //============================
    // Set up functions
    //============================
    
//    addFunction("importDirectoryListing", importDirectoryListing);
    
    //============================
    // Set up members
    //============================
    
    setMember("pathArray", ArrayObject());
    
  }


}
