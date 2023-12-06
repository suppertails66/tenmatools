#include "classes/FileListing.h"
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
    // default target: root directory
    std::string targetFolder = "";
//    if (args.size() >= 1) targetFolder = args.at(1).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Importing directory listing from "
        << "\"" << inputFolder << "\""
        << " to "
        << "\"" << targetFolder << "\"" << std::endl;
    }
    
    indexDirectory(env, self, &(self->getMember("directoryListing")),
                   inputFolder, targetFolder);
    
    if (config.debugOutput()) {
      std::cout << "Finished importing directory listing"
        << std::endl;
    }
    
    return NULL;
  } */
  
  /**
   * Generates a directory record from a FileListing.
   *
   * arg0 dst String to which to append the directory record.
   * arg1 format String identifier of CD format (e.g. "CDROM" or "XA").
   */
/*  static ParseNode* writeDirectoryRecord(
      Object* env, Object* self, ObjectArray args) {
    std::string& dst = args.at(0).stringValue();
    std::string formatString = args.at(1).stringValue();
    
    
    
    return NULL;
  } */
  
  enum SortFuncResult {
    sort_lessThan,
    sort_greaterThan,
    sort_equal
  };
  
  struct StringExtensionResult {
    bool hasExtension;
    std::string filename;
    std::string extension;
  };
  
  static StringExtensionResult getExtension(std::string str) {
    StringExtensionResult result;
    
    // initially assume no extension
    result.hasExtension = false;
    result.filename = str;
    
    for (int i = str.size() - 1; i >= 0; i--) {
      if (str[i] == '.') {
        result.hasExtension = true;
        result.filename = str.substr(0, i);
        result.extension = str.substr(i + 1, std::string::npos);
        break;
      }
    }
    
    return result;
  }
  
  static SortFuncResult sortByIso9660NameSub(
      const std::string& first, const std::string& second) {
    std::string firstC = first;
    std::string secondC = second;
    while (firstC.size() < secondC.size()) firstC += ' ';
    while (firstC.size() > secondC.size()) secondC += ' ';
    
    for (unsigned int i = 0; i < firstC.size(); i++) {
      firstC[i] = toupper(firstC[i]);
      secondC[i] = toupper(secondC[i]);
      if ((unsigned char)firstC[i] < (unsigned char)secondC[i])
        return sort_lessThan;
      else if ((unsigned char)firstC[i] > (unsigned char)secondC[i])
        return sort_greaterThan;
    }
    
    return sort_equal;
  }
  
  bool FileListing::sortByIso9660Name(const Object& first,
                                const Object& second) {
    // FIXME: according to the spec, we should process the filename
    //        and the extension separately by this same rule
    //        plus a bunch of others for more obscure cases
    // TODO: is current implementation any different from just comparing
    //       the entire name?
    
    // check for self/parent references, which are listed under their "real"
    // names but must treated as 00/01
    
    std::string firstC;
    if (first.getMemberInt("isSelfReference") != 0)
      firstC += CdConsts::directoryRecordSelfReferenceChar;
    else if (first.getMemberInt("isParentReference") != 0)
      firstC += CdConsts::directoryRecordParentReferenceChar;
    else
      firstC = first.getMemberString("name");
    
    std::string secondC;
    if (second.getMemberInt("isSelfReference") != 0)
      secondC += CdConsts::directoryRecordSelfReferenceChar;
    else if (second.getMemberInt("isParentReference") != 0)
      secondC += CdConsts::directoryRecordParentReferenceChar;
    else
      secondC = second.getMemberString("name");
    
    // obtain extensions for both names
    StringExtensionResult firstExt = getExtension(firstC);
    StringExtensionResult secondExt = getExtension(secondC);
    
//    std::string firstC = first.getMemberString("name");
//    std::string secondC = second.getMemberString("name");
    
/*    while (firstC.size() < secondC.size()) firstC += ' ';
    while (firstC.size() > secondC.size()) secondC += ' ';
    
    for (unsigned int i = 0; i < firstC.size(); i++) {
      firstC[i] = toupper(firstC[i]);
      secondC[i] = toupper(secondC[i]);
      if ((unsigned char)firstC[i] < (unsigned char)secondC[i])
        return true;
      else if ((unsigned char)firstC[i] > (unsigned char)secondC[i])
        return false;
    }
    
    return false; */
    
    // sort by filename
    SortFuncResult sortResult
      = sortByIso9660NameSub(firstExt.filename, secondExt.filename);
//    std::cerr << "name: compared " 
//      << firstExt.filename
//      << " and "
//      << secondExt.filename
//      << "; result was "
//      << sortResult << std::endl;
    
    if (sortResult == sort_lessThan) return true;
    else if (sortResult == sort_greaterThan) {
//      if (!firstExt.hasExtension || !secondExt.hasExtension)
        return false;
    }
    
    // sub-sort by extension
    sortResult
      = sortByIso9660NameSub(firstExt.extension, secondExt.extension);
//    std::cerr << "ext: compared " 
//      << firstExt.extension
//      << " and "
//      << secondExt.extension
//      << "; result was "
//      << sortResult << std::endl;
    
    if (sortResult == sort_lessThan) return true;
    else return false;
  }
  
  bool FileListing::sortByIso9660NameP(const Object* first,
                                      const Object* second) {
    return sortByIso9660Name(*first, *second);
  }
  
  /**
   * Configures the type bits of the XA submode field for a given type.
   * Has no effect if disc is not in CD-XA format, or the file is assigned
   * to a non-MODE2 sector.
   * 
   * arg0 typeString Type identifier string.
   *                 May be "VIDEO", "AUDIO", or "DATA".
   */
  static ParseNode* setXaSubmodeType(
      Object* env, Object* self, ObjectArray args) {
    std::string typeString = args.at(0).stringValue();
    std::string origTypeString = typeString;
    
    if (config.debugOutput()) {
      std::cout << "Setting XA submode type of "
        << "\"" << self->getMemberString("name") << "\""
        << " to "
        << "\"" << typeString << "\"" << std::endl;
    }
    
    TStringConversion::decapitalize(typeString);
    
    // clear all existing type flags
    self->setMember("xa_submode_data", 0);
    self->setMember("xa_submode_video", 0);
    self->setMember("xa_submode_audio", 0);
    
    // set the appropriate flag for this type
    if (typeString.compare("video") == 0) {
      self->setMember("xa_submode_video", 1);
    }
    else if (typeString.compare("audio") == 0) {
      self->setMember("xa_submode_audio", 1);
    }
    else if (typeString.compare("data") == 0) {
      self->setMember("xa_submode_data", 1);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "setXaSubmodeType()",
                              std::string("Unknown XA submode type: ")
                                + origTypeString);
    }
    
    return NULL;
  }
  
  /**
   * Generates a directory record from a FileListing.
   *
   * @arg dst TStream to write data to.
   * @arg cdFormat Format of CD (e.g. CD-ROM or CD-XA).
   */
  void FileListing::writeDirectoryRecord(
      const Object* env, const Object* self, BlackT::TStream& dst,
      CdFormatIds::CdFormatId cdFormat) {
    
    DiscasterString outputName = self->getMemberString("name");
    
    // add semicolon + version number to filename
    // if not a directory (including self/parent entries of 0x00/0x01)
    if (!(
//          ((outputName.size() == 1) && (outputName[0] == '\x00'))
//          || ((outputName.size() == 1) && (outputName[0] == '\x01'))
//          ||
          (self->getMemberInt("isDirectory") != 0)
         )
       ) {
      outputName += (std::string(";")
        + TStringConversion::intToString(self->getMemberInt("fileVersion")));
    }
    
    // special names for self-reference/parent reference
    // (which should always be directories)
    if (self->getMemberInt("isSelfReference") != 0) {
      // make sure c-string conversions don't do anything funny to our \x00
      outputName.clear();
      outputName += CdConsts::directoryRecordSelfReferenceChar;
//      outputName.assign(CdConsts::directoryRecordSelfReferenceName,
//                        sizeof(CdConsts::directoryRecordSelfReferenceName));
    }
    else if (self->getMemberInt("isParentReference") != 0) {
      outputName.clear();
      outputName += CdConsts::directoryRecordParentReferenceChar;
//      outputName.assign(CdConsts::directoryRecordParentReferenceName,
//                        sizeof(CdConsts::directoryRecordParentReferenceName));
    }
    
    // if number of bytes in name is even, we have to pad with an extra byte
    // to keep the directory record's size even
    int namePaddingSize = ((outputName.size() & 0x01) == 0) ? 1 : 0;
    
    // total size of record, including XA attributes
    int totalSize = 33 + outputName.size() + namePaddingSize;
    if (cdFormat == CdFormatIds::cdxa) {
      std::string signature = self->getMemberString("xa_signature");
      std::string reserved = self->getMemberString("xa_reserved");
      
      // just in case the user is a real joker who decided to establish their
      // very own XA variant with a differently-sized header: we support it!
      totalSize += 7 + signature.size() + reserved.size();
    }
    
    dst.writeu8(totalSize);
    // TODO: extended attribute records not implemented
    dst.writeu8((self->getMemberInt("hasExtendedAttributeRecord") == 0)
                  ? 0 : 0);
    dst.writeu32lebe(self->getMemberInt("dataOffset"));
    dst.writeu32lebe(self->getMemberInt("dataSize"));
    
    // convert and write time
    DiscasterInt recordingTime = self->getMemberInt("recordingTime");
    tm* convTime = localtime(&recordingTime);
    dst.writeu8(convTime->tm_year);
    // tm is 0-11, cdrom wants 1-12
    dst.writeu8(convTime->tm_mon + 1);
    dst.writeu8(convTime->tm_mday);
    dst.writeu8(convTime->tm_hour);
    dst.writeu8(convTime->tm_min);
    // "tm_sec is generally 0-59. The extra range [0-61] is to accommodate for
    //  leap seconds in certain systems."
    dst.writeu8((convTime->tm_sec > 59) ? 59 : convTime->tm_sec);
    // note: signed
    dst.writes8(self->getMemberInt("gmtOffset"));
    
    // generate and write file flags
    TByte fileFlags = 0;
    if (self->getMemberInt("existenceFlag") != 0)
      fileFlags |= (0x01 << 0);
    if (self->getMemberInt("isDirectory") != 0)
      fileFlags |= (0x01 << 1);
    if (self->getMemberInt("associatedFileFlag") != 0)
      fileFlags |= (0x01 << 2);
    if (self->getMemberInt("recordFlag") != 0)
      fileFlags |= (0x01 << 3);
    if (self->getMemberInt("protectionFlag") != 0)
      fileFlags |= (0x01 << 4);
    if (self->getMemberInt("reservedFlag5") != 0)
      fileFlags |= (0x01 << 5);
    if (self->getMemberInt("reservedFlag6") != 0)
      fileFlags |= (0x01 << 6);
    if (self->getMemberInt("multiExtentFlag") != 0)
      fileFlags |= (0x01 << 7);
    dst.writeu8(fileFlags);
    
    dst.writeu8(self->getMemberInt("fileUnitSize"));
    dst.writeu8(self->getMemberInt("interleaveGapSize"));
    dst.writeu16lebe(self->getMemberInt("volumeSequenceNumber"));
    dst.writeu8(outputName.size());
    // name may include null bytes (for e.g. self entry of 0x00),
    // so we can't do a c-string copy
    dst.write(outputName.c_str(), outputName.size());
    // add padding byte if name length even
    for (int i = 0; i < namePaddingSize; i++) dst.put(0x00);
    
    // XA data
    if (cdFormat == CdFormatIds::cdxa) {
      dst.writeu16be(self->getMemberInt("xa_ownerIdGroup"));
      dst.writeu16be(self->getMemberInt("xa_ownerIdUser"));
      
      // attribute flags
      int xaAttrFlags = 0;
      if (self->getMemberInt("xa_flags_ownerRead") != 0)
        xaAttrFlags |= (0x01 << 0);
      if (self->getMemberInt("xa_flags_reserved1") != 0)
        xaAttrFlags |= (0x01 << 1);
      if (self->getMemberInt("xa_flags_ownerExec") != 0)
        xaAttrFlags |= (0x01 << 2);
      if (self->getMemberInt("xa_flags_reserved3") != 0)
        xaAttrFlags |= (0x01 << 3);
      if (self->getMemberInt("xa_flags_groupRead") != 0)
        xaAttrFlags |= (0x01 << 4);
      if (self->getMemberInt("xa_flags_reserved5") != 0)
        xaAttrFlags |= (0x01 << 5);
      if (self->getMemberInt("xa_flags_groupExec") != 0)
        xaAttrFlags |= (0x01 << 6);
      if (self->getMemberInt("xa_flags_reserved7") != 0)
        xaAttrFlags |= (0x01 << 7);
      if (self->getMemberInt("xa_flags_worldRead") != 0)
        xaAttrFlags |= (0x01 << 8);
      if (self->getMemberInt("xa_flags_reserved9") != 0)
        xaAttrFlags |= (0x01 << 9);
      if (self->getMemberInt("xa_flags_worldExec") != 0)
        xaAttrFlags |= (0x01 << 10);
      if (self->getMemberInt("xa_flags_isMode2") != 0)
        xaAttrFlags |= (0x01 << 11);
      if (self->getMemberInt("xa_flags_isMode2Form2") != 0)
        xaAttrFlags |= (0x01 << 12);
      if (self->getMemberInt("xa_flags_isInterleaved") != 0)
        xaAttrFlags |= (0x01 << 13);
      if (self->getMemberInt("xa_flags_isCdda") != 0)
        xaAttrFlags |= (0x01 << 14);
      if (self->getMemberInt("isDirectory") != 0)
        xaAttrFlags |= (0x01 << 15);
      dst.writeu16be(xaAttrFlags);
      
      std::string signature = self->getMemberString("xa_signature");
      std::string reserved = self->getMemberString("xa_reserved");
      dst.write(signature.c_str(), signature.size());
      dst.writeu8(self->getMemberInt("xa_fileNumber"));
      dst.write(reserved.c_str(), reserved.size());
    }
  }
  
  Object* FileListing::getDirectoryMap(Object* env, Object* self) {
    return &(self->getMember("directoryMap"));
  }
  
  Object* FileListing
    ::addToDirectoryMap(Object* env, Object* self,
                        std::string name, Object* newObj) {
    Object& directoryMap = *getDirectoryMap(env, self);
    
    directoryMap.setMember(name, *newObj);
    
    return &(directoryMap.getMember(name));
  }
  
  bool FileListing
    ::hasFile(Object* env, Object* self, std::string name) {
    Object& directoryMap = *getDirectoryMap(env, self);
    return directoryMap.hasMember(name);
  }
  
  Object* FileListing
    ::getFile(Object* env, Object* self, std::string name) {
    Object& directoryMap = *getDirectoryMap(env, self);
    return &(directoryMap.getMember(name));
  }
  
  Object* FileListing::addFile(Object* env, Object* self,
                         std::string name, Object& newFile) {
    return addToDirectoryMap(env, self, name, &newFile);
  }
  
  Object* FileListing::getObjectFromPath(
      Object* env, Object* self, const std::string& path) {
    TFileManip::NameCollection components;
    TFileManip::splitPath(path, components);
    
    if (components.size() == 0) {
//      if (path.size() > 1) {
        throw TGenericException(T_SRCANDLINE,
                                "getObjectFromPath()",
                                std::string("Invalid target file: ")
                                  + "\""
                                  + path
                                  + "\"");
//      }
    }
    
    // final entry is filename/dirname
    std::string name = components[components.size() - 1];
    
    // traverse directories
//    Object* directoryObj = &(self->getMember("directoryListing"));
    Object* directoryObj = self;
    for (int i = 0; i < components.size() - 1; i++) {
//      directoryObj = &(directoryObj->getMember(components[i]));
      directoryObj = FileListing::getFile(env, directoryObj, components[i]);
    }
    
//    return &(directoryObj->getMember(name));
    return FileListing::getFile(env, directoryObj, name);
  }
  
  Object* FileListing::addObjectToPath(
      Object* env, Object* self, const std::string& path, Object* newObj) {
    TFileManip::NameCollection components;
    TFileManip::splitPath(path, components);
    
    if (components.size() == 0) {
      throw TGenericException(T_SRCANDLINE,
                              "addObjectToPath()",
                              std::string("Invalid target file: ")
                                + "\""
                                + path
                                + "\"");
    }
    
    // final entry is filename/dirname
    std::string name = components[components.size() - 1];
    
    // traverse directories
//    Object* directoryObj = &(self->getMember("directoryListing"));
    Object* directoryObj = self;
    for (int i = 0; i < components.size() - 1; i++) {
//      if (!directoryObj->hasMember(components[i])) {
      if (!FileListing::hasFile(env, directoryObj, components[i])) {
//        directoryObj->setMember(components[i],
//                                FileListing(components[i], true));
        Object newDir = FileListing(components[i], true);
        FileListing::addFile(env, directoryObj, components[i], newDir);
      }
//      directoryObj = &(directoryObj->getMember(components[i]));
      directoryObj = FileListing::getFile(env, directoryObj, components[i]);
    }
    
    // final entry is filename
//    directoryObj->setMember(name, *newObj);
    FileListing::addFile(env, directoryObj, name, *newObj);
    
//    return &(directoryObj->getMember(name));
    return FileListing::getFile(env, directoryObj, name);
  }

  FileListing::FileListing()
    : BlobObject("FileListing") {
    defaultInit();
  }
  
  FileListing::FileListing(std::string name, bool isDirectory)
    : BlobObject("FileListing") {
    defaultInit();
    setMember("name", name);
    setMember("isDirectory", (isDirectory ? 1 : 0));
  }
  
  void FileListing::defaultInit() {
    //============================
    // Set up functions
    //============================
    
    addFunction("setXaSubmodeType", setXaSubmodeType);
    
    //============================
    // Set up members
    //============================
    
      // note: not a DirectoryListing() itself. otherwise, this would
      // be an infinite recursion.
      setMember("directoryMap", BlobObject("NameFileListingMap"));
      setMember("parentPath", "");
      setMember("sourceFilename", "");
      // for deciding whether this is a self-reference or parent reference
      setMember("isSelfReference", 0);
      setMember("isParentReference", 0);
      
      //===========================================
      // standard CD directory record properties
      //===========================================
      
      // TODO: extended attribute records not implemented, hopefully never will be
      setMember("hasExtendedAttributeRecord", 0);
      setMember("dataOffset", 0);
      setMember("dataSize", 0);
      setMember("recordingTime", 0);  // note: unix time in our representation
      setMember("gmtOffset", 0);      // modify if you're truly mad
      
      // file flags
      
      setMember("existenceFlag", 0);
      setMember("isDirectory", 0);
      setMember("associatedFileFlag", 0);
      setMember("recordFlag", 0);
      setMember("protectionFlag", 0);
      setMember("reservedFlag5", 0);
      setMember("reservedFlag6", 0);
      setMember("multiExtentFlag", 0);
      
      // TODO: interleaving not implemented, probably will have to be
      setMember("fileUnitSize", 0);
      setMember("interleaveGapSize", 0);
      setMember("volumeSequenceNumber", 1);
      setMember("name", "");
      // the enigmatic ";1" in every filename
      setMember("fileVersion", 1);
      
      //===========================================
      // XA extensions -- only used for XA mode
      //===========================================
      
      setMember("xa_ownerIdGroup", 0);
      setMember("xa_ownerIdUser", 0);
      
      // file attribute bits from 0 to 15
      setMember("xa_flags_ownerRead", 1);
      setMember("xa_flags_reserved1", 0);
      setMember("xa_flags_ownerExec", 1);
      setMember("xa_flags_reserved3", 0);
      setMember("xa_flags_groupRead", 1);
      setMember("xa_flags_reserved5", 0);
      setMember("xa_flags_groupExec", 1);
      setMember("xa_flags_reserved7", 0);
      setMember("xa_flags_worldRead", 1);
      setMember("xa_flags_reserved9", 0);
      setMember("xa_flags_worldExec", 1);
      setMember("xa_flags_isMode2", 0);
      setMember("xa_flags_isMode2Form2", 0);
      setMember("xa_flags_isInterleaved", 0);
      setMember("xa_flags_isCdda", 0);
      // redundant with isDirectory property
  //    setMember("xa_isDirectory", 0);

      setMember("xa_signature", "XA");
      setMember("xa_fileNumber", 0);
      char xa_reserved_value[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
      std::string xa_reserved;
      xa_reserved.assign(xa_reserved_value, sizeof(xa_reserved_value));
      setMember("xa_reserved", xa_reserved);
      
      // TODO: currently ignored, may or may not stay that way.
      // if the user needs to work with interleaving, etc.,
      // it's probably best to just let them do the data preparation outside
      // of this program and then import the full 0x930-byte sectors with a raw
      // import.
      setMember("xa_submode_data", 1);
      setMember("xa_submode_video", 0);
      setMember("xa_submode_audio", 0);
      setMember("xa_submode_trigger", 0);
      setMember("xa_submode_realTime", 0);
      
      //===========================================
      // other
      //===========================================
      
      // if set, the target file consists of raw 0x930-byte sectors.
      // all data will be transferred to the disc as-is, with only the relevant
      // header data (disc MSF position and ECC/EDC) updated.
      // the format of the sectors is assumed to be the same as whatever mode
      // the disc is in at the time the file is placed on the disc.
      setMember("usesRawSectors", 0);
      
      // if set, sectors with usesRawSectors set will have ECC data recalculated
      // (if they have any)
//      setMember("recalculateRawSectorEcc", 1);
      
      // if set, sectors with usesRawSectors set will have EDC data recalculated
      // (if they have any)
//      setMember("recalculateRawSectorEdc", 1);
  }


}
