#include "classes/IsoFilesystem.h"
#include "classes/FileListing.h"
#include "classes/DiscPointerListing.h"
#include "classes/IsoPrimaryVolumeDescriptor.h"
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
#include <fstream>

using namespace BlackT;

namespace Discaster {


  /**
   * Helper function to add a build command to the build command list.
   */
  static void addBuildCommand(Object* self, const Object& command) {
    if (self != NULL) {
     self->getMember("buildList").arrayValue().push_back(command);
   }
  }
  
  static void indexDirectory(
      Object* env, Object* self, Object* targetFolderListing,
      std::string inputFolder, std::string targetFolder,
      std::string parentFolder = "") {
    
    // get root dir info
    // FIXME: probably not guaranteed to exist yet
    TFileInfo rootInfo = TFileManip::getFileInfo(inputFolder);
    Object& rootObj = *IsoFilesystem::getDirectoryListingObjectFromPath(
      env, self, targetFolder);
    rootObj.setMember("recordingTime", rootInfo.modifiedTime());
    
    // get list of folder contents
    TFileManip::FileInfoCollection fileInfoCollection;
    TFileManip::getDirectoryListing(inputFolder.c_str(), fileInfoCollection);
    std::sort(fileInfoCollection.begin(), fileInfoCollection.end(),
              TFileInfo::sortByIso9660Name);
    
    for (TFileManip::FileInfoCollection::iterator it
           = fileInfoCollection.begin();
         it != fileInfoCollection.end();
         ++it) {
      TFileInfo& info = *it;
      
      if (info.isDirectory()) {
        std::string inputSubFolderPath = info.path();
        std::string targetSubFolderPath
          = targetFolder
            + ((targetFolder.size() == 0) ? "/" : "/")
            + info.name();
//          = targetFolder;
        // root directory doesn't need slash added
//        if (targetFolder.size() > 0) targetSubFolderPath += "/";
//        targetSubFolderPath += info.name();
        
        // recursively index all subdirectories
        
        if (config.debugOutput()) {
          std::cout << "  Importing directory: " << info.path() << std::endl;
        }
        
        // create FileListing object for this folder if none exists
//        if (!targetFolderListing->hasMember(info.name())) {
        if (!FileListing::hasFile(env, targetFolderListing, info.name())) {
//          targetFolderListing->setMember(info.name(),
//            FileListing(info.name(), true));
          Object obj = FileListing(info.name(), true);
          obj.getMember("parentPath").setStringValue(targetFolder);
          FileListing::addFile(env, targetFolderListing, info.name(), obj);
          
//          std::cerr << "file: " << targetFolder << ", parent: "
//            << parentFolder << std::endl;
        }
        
//        Object* targetSubFolderListing
//          = &(targetFolderListing->getMember(info.name()));
        Object* targetSubFolderListing
          = FileListing::getFile(env, targetFolderListing, info.name());
        
        // TODO: check this
        indexDirectory(env, self, targetSubFolderListing,
                       inputSubFolderPath, targetSubFolderPath,
                       targetFolder);
                       
        if (config.debugOutput()) {
          std::cout << "  Finished importing directory: " << info.path() << std::endl;
        }
      }
      else {
        std::string targetFileName =
          targetFolder
          + ((targetFolder.size() == 0) ? "/" : "/")
          + info.name();
          
        if (config.debugOutput()) {
          std::cout << "    Importing file \"" << info.path() << "\" "
            << "as \"" << targetFileName << "\"" << std::endl;
        }
        
        Object newFileListing = FileListing(info.name(), false);
        newFileListing.getMember("parentPath").setStringValue(targetFolder);
        newFileListing.setMember("sourceFilename", info.path());
        newFileListing.setMember("dataSize", info.size());
        newFileListing.setMember("recordingTime", info.modifiedTime());
          
//          std::cerr << "file: " << targetFolder << ", parent: "
//            << parentFolder << std::endl;
        
        
//        targetFolderListing->setMember(info.name(), newFileListing);
        FileListing::addFile(env, targetFolderListing,
                             info.name(), newFileListing);
      }
    }
  }

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
  static ParseNode* importDirectoryListing(
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
  }

  /**
   * Imports a raw-sector file.
   * File contents are raw 0x930-byte sectors preformatted for the
   * specified mode.
   *
   * arg0 inputFile File on disk to import.
   * arg1 targetFile Target path on built disc.
   */
  static ParseNode* importRawSectorFile(
      Object* env, Object* self, ObjectArray args) {
    std::string inputFile = args.at(0).stringValue();
    std::string targetFile = args.at(1).stringValue();
//    std::string modestring = args.at(2).stringValue();
    
//    CdModeIds::CdModeId modeId = CdConsts::modeStringToId(modestring);
    
    if (config.debugOutput()) {
//      std::cout << "Importing raw-sector file "
//        << "\"" << inputFile << "\""
//        << " to directory listing as "
//        << "\"" << targetFile << "\""
//        << " with sector mode "
//        << modestring
//        << std::endl;
      std::cout << "Importing raw-sector file "
        << "\"" << inputFile << "\""
        << " to directory listing as "
        << "\"" << targetFile << "\""
        << std::endl;
    }
    
    // get parent directory name
    std::string targetFolder = TFileManip::getDirectoryName(targetFile);
    
    // get parent directory listing
//    Object* targetFolderListing = IsoFilesystem
//      ::getDirectoryListingObjectFromPath(env, self, targetFolder);
//    Object* targetFolderListing = &(self.getMember("directoryListing"));
    
    // get file info
    TFileInfo info = TFileManip::getFileInfo(inputFile);
    
    int rawFileSize = info.size();
    
    // size must be multiple of 0x930
    if ((rawFileSize % CdConsts::physicalSectorSize) != 0) {
      throw TGenericException(T_SRCANDLINE,
                              "importRawSectorFile()",
                              std::string("Raw-sector file ")
                                + inputFile
                                + " does not have size divisible by "
                                + TStringConversion::toString(
                                    CdConsts::physicalSectorSize));
    }
    
    // compute actual file size.
    // this is the data area size of the specified mode times the number of
    // sectors in the file.
//    int targetModeDataAreaSize = CdConsts::sectorDataAreaSize(modeId);
//    int fileDataSize = (rawFileSize / CdConsts::physicalSectorSize)
//                          * targetModeDataAreaSize;
    
    // actually, the file may use multiple modes.
    // we cannot know the data size without actually reading the mode off of
    // every sector.
    
    // ATTEMPT 1: actually evaluate the size of every sector based on mode.
    //            technically correct, but not how the original discs do it!
    
/*    if (config.debugOutput()) {
      std::cout << "Evaluating dataSize of raw-sector file "
        << "\"" << inputFile << "\": ";
    }
    
    int fileDataSize = 0;
    TIfstream ifs(inputFile.c_str(), std::ios_base::binary);
    int numRawSectors = rawFileSize / CdConsts::physicalSectorSize;
    for (int i = 0; i < numRawSectors; i++) {
      int startPos = ifs.tell();
      
      // read header + subheader only
      char buffer[CdConsts::maxHeaderAndSubheaderSize];
      ifs.read(buffer, CdConsts::maxHeaderAndSubheaderSize);
      
      CdModeIds::CdModeId mode = CdConsts::modeDataToId(
        buffer[CdConsts::sectorHeaderModeOffset],
        buffer[CdConsts::mode2SubheaderSubmodeOffset]);
      
      fileDataSize += CdConsts::sectorDataAreaSize(mode);
      
      // seek to next sector position
      ifs.seek(startPos + CdConsts::physicalSectorSize);
    }
    
    if (config.debugOutput()) {
      std::cout << std::dec << fileDataSize << " bytes" << std::endl;
    } */
    
    // ATTEMPT 2: give up and stop caring
    
    // ...honestly, do we even use this information?
//    int fileDataSize = 0;

    // ATTEMPT 3: get pedantic enough to figure out how actual discs do it:
    //            just pretend all sectors are 0x800 bytes
    
    int fileDataSize = 0;
    {
      TIfstream ifs(inputFile.c_str(), std::ios_base::binary);
      fileDataSize = (ifs.size() / CdConsts::physicalSectorSize)
                        * CdConsts::logicalSectorSize;
    }
    
    Object newFileListing = FileListing(info.name(), false);
    newFileListing.getMember("parentPath").setStringValue(targetFolder);
    newFileListing.setMember("sourceFilename", info.path());
    newFileListing.setMember("dataSize", fileDataSize);
    newFileListing.setMember("recordingTime", info.modifiedTime());
    
    newFileListing.getMember("usesRawSectors").setIntValue(1);
//    newFileListing.setMember("rawSectorModestring", modestring);
      
//    FileListing::addFile(env, targetFolderListing,
//                         info.name(), newFileListing);
    IsoFilesystem::addDirectoryListingObjectToPath(
                         env, self,
                         targetFile, &newFileListing);
    
    if (config.debugOutput()) {
      std::cout << "Finished importing raw-sector file "
        << "\"" << inputFile << "\""
        << " to directory listing as "
        << "\"" << targetFile << "\""
//        << " with sector mode "
//        << modestring
        << "; physical size was "
        << rawFileSize
        << ", computed logical size is "
        << fileDataSize
        << std::endl;
    }
    
    return NULL;
  }
  
  /**
   * Imports a file to use as the System Area.
   *
   * arg0 inputFile Folder on disk to import.
   */
  static ParseNode* setSystemArea(
      Object* env, Object* self, ObjectArray args) {
    std::string inputFile = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Importing System Area file: "
        << inputFile << std::endl;
    }
    
//    TIfstream ifs(inputFile.c_str(),  std::ios_base::binary);
    
    // actually, the limit is technically dependent on the sector mode.
    // it might theoretically be possible for the system area to be mode2form2.
    // so check this when the data is actually added to the disc image.
/*    if (ifs.size() >= CdConsts::cdRomSystemAreaSizeInBytes) {
      throw TGenericException(T_SRCANDLINE,
                              "importDirectoryListing()",
                              std::string("System Area file too large: ")
                                + inputFile
                                + " is "
                                + TStringConversion::toString(ifs.size())
                                + " bytes, max is "
                                + TStringConversion::toString(
                                    CdConsts::cdRomSystemAreaSizeInBytes));
    } */
    
    TBufStream ifs;
    ifs.open(inputFile.c_str());
    if (ifs.size() <= 0) {
      throw TGenericException(T_SRCANDLINE,
                              "setSystemArea()",
                              std::string("Error opening System Area file ")
                                + "\""
                                + inputFile
                                + "\""
                                + " (does not exist?)");
    }
    
    std::string data;
    data.assign(ifs.data().data(), ifs.size());
    self->getMember("systemArea").setStringValue(data);
    
    if (config.debugOutput()) {
      std::cout << "Imported System Area of "
        << data.size() << " bytes" << std::endl;
    }
    
    return NULL;
  }
  
  /**
   * Imports a raw-sector file to use as the System Area.
   *
   * arg0 inputFile Folder on disk to import.
   */
  static ParseNode* setRawSectorSystemArea(
      Object* env, Object* self, ObjectArray args) {
    std::string inputFile = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Importing raw-sector System Area file: "
        << inputFile << std::endl;
    }
    
    TBufStream ifs;
    ifs.open(inputFile.c_str());
    if (ifs.size() <= 0) {
      throw TGenericException(T_SRCANDLINE,
                              "setRawSectorSystemArea()",
                              std::string("Error opening raw-sector")
                                + " System Area file "
                                + "\""
                                + inputFile
                                + "\""
                                + " (does not exist?)");
    }
    
    std::string data;
    data.assign(ifs.data().data(), ifs.size());
    self->getMember("systemArea").setStringValue(data);
    self->getMember("systemAreaIsRawSector").setIntValue(1);
    
    if (config.debugOutput()) {
      std::cout << "Imported raw-sector System Area of "
        << data.size() << " bytes" << std::endl;
    }
    
    return NULL;
  }
  
  /**
   * Adds a disc pointer object to the file listing.
   * Disc pointers specify an offset in logical blocks from the start of
   * the ISO (start of the System Area) to an arbitrary area of the disc
   * identified by a start label and an end label (added via the addLabel()
   * function of a CdImage).
   * They are inserted into the filesystem of the disc like any "real" file
   * except that they do not exist within the actual ISO's data.
   * Note that if the point on the disc precedes the ISO, this will result
   * in a negative block offset, which may cause problems.
   *
   * arg0 targetFile Path from the root of the disc to the disc pointer,
   *                 including the name of the pointer.
   * arg1 startLabelName Name of the label identifying the pointer's start.
   * arg2 endLabelName Name of the label identifying the pointer's end.
   */
  static ParseNode* insertDiscPointer(
      Object* env, Object* self, ObjectArray args) {
    std::string targetFile = args.at(0).stringValue();
    std::string startLabelName = args.at(1).stringValue();
    std::string endLabelName = args.at(2).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding disc pointer \"" << targetFile << "\","
        << " starting at label \"" << startLabelName << "\""
        << " and ending at label \"" << endLabelName << "\"" << std::endl;
    }
  
    // figure out path through hierarchy to target
    TFileManip::NameCollection targetPathComponents;
    TFileManip::splitPath(targetFile, targetPathComponents);
    
    if (targetPathComponents.size() == 0) {
      throw TGenericException(T_SRCANDLINE,
                              "insertFilePointer()",
                              std::string("Invalid target directory for ")
                                + "file pointer: "
                                + "\""
                                + targetFile
                                + "\"");
    }
    
    std::string name = targetPathComponents[targetPathComponents.size() - 1];
    
/*    Object ptrObj = DiscPointerListing();
    ptrObj.setMember("name", name);
    ptrObj.setMember("isDirectory", 0);
    ptrObj.setMember("startLabelName", startLabelName);
    ptrObj.setMember("endLabelName", endLabelName);
    
    // traverse directories
    Object* directoryObj = &(self->getMember("directoryListing"));
    for (int i = 0; i < targetPathComponents.size() - 1; i++) {
      if (!directoryObj->hasMember(targetPathComponents[i])) {
        directoryObj->setMember(targetPathComponents[i],
                                FileListing(targetPathComponents[i], true));
      }
      directoryObj = &(directoryObj->getMember(targetPathComponents[i]));
    }
    
    // final entry is filename
    directoryObj->setMember(name, ptrObj); */
    
    Object ptrObj = DiscPointerListing();
    ptrObj.setMember("name", name);
    ptrObj.setMember("isDirectory", 0);
    ptrObj.setMember("startLabelName", startLabelName);
    ptrObj.setMember("endLabelName", endLabelName);
    
    FileListing::addObjectToPath(env, &(self->getMember("directoryListing")),
                                 targetFile, &ptrObj);
    
    return NULL;
  }
  
  /**
   * Sets ISO format.
   * Valid values are "CDROM" (default) and "CDXA".
   *
   * arg0 formatName Name of the format.
   */
  static ParseNode* setFormat(
      Object* env, Object* self, ObjectArray args) {
    std::string formatName = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Setting disc format to \""
        << formatName << "\"" << std::endl;
    }
    
    self->setMember("format", formatName);
    
    return NULL;
  }
  
  /**
   * Adds primary volume descriptor to build list.
   *
   * arg0 primaryVolumeDescriptor The primary volume descriptor.
   */
  static ParseNode* addPrimaryVolumeDescriptor(
      Object* env, Object* self, ObjectArray args) {
    const Object& primaryVolumeDescriptor = args.at(0);
    
    if (config.debugOutput()) {
      std::cout << "Adding primary volume descriptor with volume identifier \""
        << primaryVolumeDescriptor.getMemberString("volumeIdentifier")
        << "\""
        << std::endl;
    }

    // copy in the descriptor
    Object command = BlobObject("addPrimaryVolumeDescriptor");
    command.setMember("primaryVolumeDescriptor", primaryVolumeDescriptor);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  /**
   * Adds descriptor set terminator to build list.
   *
   * arg0 descriptorSetTerminator (Optional) The descriptor set terminator.
   *                              If not specified, the default terminator
   *                              is used.
   */
  static ParseNode* addDescriptorSetTerminator(
      Object* env, Object* self, ObjectArray args) {
//    const Object& descriptorSetTerminator = args.at(0);
    Object term = IsoDescriptorSetTerminator();
    if (args.size() > 0) {
     term = args.at(0);
    }
    
    const Object& descriptorSetTerminator = term;
    
    if (config.debugOutput()) {
      std::cout << "Adding descriptor set terminator" << std::endl;
    }

    // copy in the descriptor
    Object command = BlobObject("addDescriptorSetTerminator");
    command.setMember("descriptorSetTerminator", descriptorSetTerminator);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  /**
   * Adds directory descriptor to build list.
   *
   * arg0 directoryName The target directory to create a descriptor for.
   *                    Must appear in the directory listing at build time.
   */
  static ParseNode* addDirectoryDescriptor(
      Object* env, Object* self, ObjectArray args) {
    std::string directoryName = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding directory descriptor for directory "
        << "\""
        << directoryName
        << "\""
        << std::endl;
    }
    
    Object command = BlobObject("addDirectoryDescriptor");
    command.setMember("directoryName", directoryName);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  /**
   * Adds a physical file placement to the build list.
   * The file will be written to the disc when this command is evaluated.
   * The file must appear in the directory listing at the given path.
   *
   * arg0 filename The target file to place on disc.
   *               Must appear in the directory listing at build time.
   */
  static ParseNode* addListedFile(
      Object* env, Object* self, ObjectArray args) {
    std::string filename = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding listed file for placement: "
        << "\""
        << filename
        << "\""
        << std::endl;
    }
    
    Object command = BlobObject("addListedFile");
    command.setMember("filename", filename);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  static ParseNode* addNullSectors(
      Object* env, Object* self, ObjectArray args) {
    int numSectors = args.at(0).intValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding " << numSectors
        << " null sector(s) for ISO placement"
        << std::endl;
    }
    
    Object command = BlobObject("addNullSectors");
    command.setMember("numSectors", numSectors);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  /**
   * Adds a type L path table to the build list.
   * Contents are automatically generated from the directory listing.
   */
  static ParseNode* addTypeLPathTable(
      Object* env, Object* self, ObjectArray args) {
//    std::string filename = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding Type L Path Table for placement"
        << std::endl;
    }
    
    Object command = BlobObject("addTypeLPathTable");
//    command.setMember("filename", filename);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  /**
   * Adds a copy of the type L path table to the build list.
   */
  static ParseNode* addTypeLPathTableCopy(
      Object* env, Object* self, ObjectArray args) {
//    std::string filename = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding Type L Path Table copy for placement"
        << std::endl;
    }
    
    Object command = BlobObject("addTypeLPathTableCopy");
//    command.setMember("filename", filename);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  /**
   * Adds a type M path table to the build list.
   * Contents are automatically generated from the directory listing.
   */
  static ParseNode* addTypeMPathTable(
      Object* env, Object* self, ObjectArray args) {
//    std::string filename = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding Type M Path Table for placement"
        << std::endl;
    }
    
    Object command = BlobObject("addTypeMPathTable");
//    command.setMember("filename", filename);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  /**
   * Adds a copy of the type M path table to the build list.
   */
  static ParseNode* addTypeMPathTableCopy(
      Object* env, Object* self, ObjectArray args) {
//    std::string filename = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding Type M Path Table copy for placement"
        << std::endl;
    }
    
    Object command = BlobObject("addTypeMPathTableCopy");
//    command.setMember("filename", filename);

    addBuildCommand(self, command);
    
    return NULL;
  }
  
  void addDirectoryContentFullyStep(
      Object* env, Object* self, std::string path) {
    
    Object& directoryListing
      = *IsoFilesystem::getDirectoryListingObjectFromPath(env, self, path);
      
    if (directoryListing.getMemberInt("isDirectory") == 0) {
      throw TGenericException(T_SRCANDLINE,
                              "addDirectoryContentFullyStep()",
                              std::string("Tried to add directory content of ")
                                + "non-directory "
                                + "\""
                                + path
                                + "\"");
    }
      
    // add descriptor command for directory
    {
      ObjectArray args;
      args.push_back(StringObject(path));
      addDirectoryDescriptor(env, self, args);
    }
    
    Object& directoryMap
      = *FileListing::getDirectoryMap(env, &directoryListing);
    
    // sort by iso9660 name
    std::vector<Object*> sortVec;
    for (ObjectMemberMap::iterator it = directoryMap.members().begin();
         it != directoryMap.members().end();
         ++it) {
      sortVec.push_back(&(it->second));
    }
    
    std::sort(sortVec.begin(), sortVec.end(), FileListing::sortByIso9660NameP);
    
/*    for (ObjectMemberMap::iterator it = directoryMap.members().begin();
         it != directoryMap.members().end();
         ++it) {
       
    } */
    
    
    for (std::vector<Object*>::iterator it = sortVec.begin();
         it != sortVec.end();
         ++it) {
      Object& fileObj = *(*it);
      
      std::string filePath = path + "/" + fileObj.getMemberString("name");
      
      if (fileObj.getMemberInt("isDirectory") != 0) {
        // add descriptor command for directory
        ObjectArray args;
        args.push_back(StringObject(filePath));
        addDirectoryDescriptor(env, self, args);
        
        // recursively handle directory contents
        addDirectoryContentFullyStep(
          env, self, filePath);
      }
      else {
        // add placement command for file
        ObjectArray args;
        args.push_back(StringObject(filePath));
        addListedFile(env, self, args);
      }
    }
  }
  
  /**
   * Recursively adds all directory descriptors and file content
   * in a given listed directory for physical placement on the disc.
   * Traversing alphabetically through the given directory,
   * files are added for placement and directories are marked for
   * descriptor placement in alphabetical order (according to
   * ISO 9660 alphabetization rules).
   *
   * arg0 path Path to a listed directory.
   */
  static ParseNode* addDirectoryContentFully(
      Object* env, Object* self, ObjectArray args) {
    std::string path = args.at(0).stringValue();
    
    if (config.debugOutput()) {
      std::cout << "Adding all contents of "
        << "\""
        << path
        << "\""
        << " for placement"
        << std::endl;
    }
    
//    Object command = BlobObject("addDirectoryContentFully");
//    command.setMember("path", path);

//    addBuildCommand(self, command);

    addDirectoryContentFullyStep(env, self, path);
    
    return NULL;
  }
  
  /**
   * Returns a ParseNode OBJECTP pointing to the file listing for a path.
   *
   * arg0 path Path to an extant file or directory.
   */
  static ParseNode* getListedFile(
      Object* env, Object* self, ObjectArray args) {
    std::string path = args.at(0).stringValue();
    
    Object& fileObj = *IsoFilesystem::getDirectoryListingObjectFromPath(
                          env, self, path);
    
    ParseNode* result = new ParseNode("OBJECTP", -1);
    result->setVoidp(&fileObj);
    
    return result;
  }
  
  Object* IsoFilesystem::getDirectoryListingObjectFromPath(
      Object* env, Object* self, const std::string& path) {
    Object* base = &(self->getMember("directoryListing"));
    
    // check for root references
    if ((path.size() == 0)
        || ((path.size() == 1)
            && (path[0] == '/'))) {
      return base;
    }
    
    return FileListing::getObjectFromPath(env, base, path);
  }
  
  Object* IsoFilesystem::addDirectoryListingObjectToPath(
      Object* env, Object* self, const std::string& path, Object* newObj) {
    Object* base = &(self->getMember("directoryListing"));
    return FileListing::addObjectToPath(env, base, path, newObj);
  }
  
  static void addSelfAndParentPointersStep(Object* env, Object* self,
                                           Object* fileListing) {
    // recursively handle all subdirectories
    Object& directoryMap = *FileListing::getDirectoryMap(env, fileListing);
    for (ObjectMemberMap::iterator it = directoryMap.members().begin();
         it != directoryMap.members().end();
         ++it) {
      Object& subFileListing = it->second;
      if ((subFileListing.getMemberInt("isDirectory") != 0)
          && (subFileListing.getMemberInt("isSelfReference") == 0)
          && (subFileListing.getMemberInt("isParentReference") == 0)) {
        addSelfAndParentPointersStep(env, self, &subFileListing);
      }
    }
    
    std::string selfName = fileListing->getMemberString("name");
    std::string parentName = fileListing->getMemberString("parentPath");
    
    if (config.debugOutput()) {
      std::cout << "Adding self/parent pointer entries to directory "
        << "\""
        << selfName
        << "\""
        << " (parent: "
        << "\""
        << parentName
        << "\""
        << ")" 
        << std::endl;
    }
  
    // every folder is required to provide a directory record
    // that points to itself and one that points to its parent
    // (with the root parented to itself).
    // the names listed in the objects are the "actual" names so we know what
    // they're pointing to, but they're mapped to the dummy members 00 and 01
    // so that they don't conflict with other names.
    
//    std::cerr << "selfName: " << selfName << std::endl;
//    std::cerr << "parentName: " << parentName << std::endl;

    std::string actualSelfName = selfName;
    // FIXME: ugly
    if (actualSelfName.compare(std::string("\x00", 1)) == 0) actualSelfName = "";

    Object& actualSelfObj = *IsoFilesystem::getDirectoryListingObjectFromPath(
                        env, self, parentName + "/" + actualSelfName);
    Object& actualParentObj = *IsoFilesystem::getDirectoryListingObjectFromPath(
                        env, self, parentName);
    
    std::string selfIdStr;
    selfIdStr += CdConsts::directoryRecordSelfReferenceChar;
//    Object selfObj = FileListing(selfName, true);
    Object selfObj = actualSelfObj;
    selfObj.getMember("isSelfReference").setIntValue(1);
    FileListing::addFile(env, fileListing, selfIdStr, selfObj);
    
    std::string parentIdStr;
    parentIdStr += CdConsts::directoryRecordParentReferenceChar;
//    Object parentObj = FileListing(parentName, true);
    Object parentObj = actualParentObj;
    parentObj.getMember("isParentReference").setIntValue(1);
    FileListing::addFile(env, fileListing, parentIdStr, parentObj);
  }
  
  void IsoFilesystem::addSelfAndParentPointers(
      Object* env, Object* self) {
    addSelfAndParentPointersStep(env, self,
                                 &(self->getMember("directoryListing")));
  }
  
  IsoFilesystem::IsoFilesystem()
    : BlobObject() {
    //============================
    // Set up functions
    //============================
    
    addFunction("importDirectoryListing", importDirectoryListing);
    addFunction("importRawSectorFile", importRawSectorFile);
    addFunction("setSystemArea", setSystemArea);
    addFunction("setRawSectorSystemArea", setRawSectorSystemArea);
    addFunction("insertDiscPointer", insertDiscPointer);
    addFunction("setFormat", setFormat);
    addFunction("addPrimaryVolumeDescriptor", addPrimaryVolumeDescriptor);
    addFunction("addDescriptorSetTerminator", addDescriptorSetTerminator);
    addFunction("addDirectoryDescriptor", addDirectoryDescriptor);
    addFunction("addListedFile", addListedFile);
    addFunction("addNullSectors", addNullSectors);
    addFunction("addTypeLPathTable", addTypeLPathTable);
    addFunction("addTypeLPathTableCopy", addTypeLPathTableCopy);
    addFunction("addTypeMPathTable", addTypeMPathTable);
    addFunction("addTypeMPathTableCopy", addTypeMPathTableCopy);
    addFunction("addDirectoryContentFully", addDirectoryContentFully);
    addFunction("getListedFile", getListedFile);
    
    //============================
    // Set up members
    //============================
    
    // array of build commands, executed sequentially upon disc export
    setMember("buildList", ArrayObject());
    // hierarchy of files available for placement on disc.
    // this member represents the root directory; its members represent files
    // and subdirectories of the root
    Object rootDirectoryListing = FileListing("", true);
    // TODO: is this ok?
    std::string selfIdStr;
    selfIdStr += CdConsts::directoryRecordSelfReferenceChar;
    rootDirectoryListing.setMember("name", selfIdStr);
    rootDirectoryListing.setMember("parentPath", "");
    setMember("directoryListing", rootDirectoryListing);
    // the 16-sector (max) system area of the disc, stored as a string
    setMember("systemArea", StringObject());
    // flag indicating whether or not system area string uses raw 0x930-byte
    // sectors
    setMember("systemAreaIsRawSector", 0);
    // TODO: only 0x800 supported for now
    setMember("logicalBlockSize", 0x800);
    // standard CD-ROM/CD-XA selector
    setMember("format", "CDROM");
  }


}
