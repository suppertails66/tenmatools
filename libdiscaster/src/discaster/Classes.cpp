#include "discaster/Classes.h"
#include "discaster/Object.h"
#include "classes/CdImage.h"
#include "classes/IsoFilesystem.h"
#include "classes/FileListing.h"
#include "classes/DiscPointerListing.h"
#include "classes/IsoPrimaryVolumeDescriptor.h"
#include "classes/IsoDescriptorSetTerminator.h"
#include "classes/IsoPathTable.h"
#include "classes/PathTableEntry.h"

namespace Discaster {


  static Object intConstructor() {
    return IntObject();
  }
  
  static Object stringConstructor() {
    return StringObject();
  }
  
  static Object arrayConstructor() {
    return ArrayObject();
  }
  
  static Object functionPointerConstructor() {
    return FunctionPointerObject();
  }
  
  static Object blobConstructor() {
    return BlobObject();
  }
  
  static Object cdImageConstructor() {
    return CdImage();
  }
  
  static Object isoFilesystemConstructor() {
    return IsoFilesystem();
  }
  
  static Object fileListingConstructor() {
    return FileListing();
  }
  
  static Object discPointerListingConstructor() {
    return DiscPointerListing();
  }
  
  static Object isoPrimaryVolumeDescriptorConstructor() {
    return IsoPrimaryVolumeDescriptor();
  }
  
  static Object isoDescriptorSetTerminatorConstructor() {
    return IsoDescriptorSetTerminator();
  }
  
  static Object isoPathTableConstructor() {
    return IsoPathTable();
  }
  
  static Object pathTableEntryConstructor() {
    return PathTableEntry();
  }
  
  /**
   * Master table of classes.
   * Used by parser to detect variable declaration keywords.
   * All of these are reserved words in the language.
   */
  const MasterClassTableEntry MasterClassTable::masterClassTable[] = {
    { "int",                        intConstructor },
    { "string",                     stringConstructor },
    { "array",                      arrayConstructor },
    { "function",                   functionPointerConstructor },
    { "blob",                       blobConstructor },
    { "CdImage",                    cdImageConstructor },
    { "IsoFilesystem",              isoFilesystemConstructor },
    { "FileListing",                fileListingConstructor },
    { "DiscPointerListing",         discPointerListingConstructor },
    { "IsoPrimaryVolumeDescriptor", isoPrimaryVolumeDescriptorConstructor },
    { "IsoDescriptorSetTerminator", isoDescriptorSetTerminatorConstructor },
    { "IsoPathTable",               isoPathTableConstructor },
    { "PathTableEntry",             pathTableEntryConstructor }
  };
  
  const int MasterClassTable::numEntries() {
    return sizeof(masterClassTable) / sizeof(MasterClassTableEntry);
  }
  
  const MasterClassTableEntry& MasterClassTable::entry(int index) {
    return masterClassTable[index];
  }
  
  
}
