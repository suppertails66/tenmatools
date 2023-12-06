#include "classes/IsoPrimaryVolumeDescriptor.h"
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
   * Converts a Unix time to a Primary Volume Descriptor timestring.
   *
   * @arg timeVal A Unix time.
   * @return Primary Volume Descriptor-formatted timestring.
   */
  static std::string toPvdTimestring(std::time_t timeVal) {
//    std::string result;
    char resultraw[18];
    
    // convert and write time
    // (filler zero values for centiseconds and timezone)
    tm* convTime = localtime(&timeVal);
    snprintf(resultraw, sizeof(resultraw), "%04d%02d%02d%02d%02d%02d000",
             convTime->tm_year + 1900,
             convTime->tm_mon + 1,
             convTime->tm_mday,
             convTime->tm_hour + 1,
             convTime->tm_min + 1,
             convTime->tm_sec + 1);
    std::string result(resultraw);
  
//    result += TStringConversion::toString(1900 + convTime->tm_year);
/*    dst.writeu8(convTime->tm_year);
    // tm is 0-11, cdrom wants 1-12
    dst.writeu8(convTime->tm_mon + 1);
    dst.writeu8(convTime->tm_mday);
    dst.writeu8(convTime->tm_hour);
    dst.writeu8(convTime->tm_min);
    // "tm_sec is generally 0-59. The extra range [0-61] is to accommodate for
    //  leap seconds in certain systems."
    dst.writeu8((convTime->tm_sec > 59) ? 59 : convTime->tm_sec);
    // note: signed
 //   dst.writes8(self->getMemberInt("gmtOffset")); */
    
    return result;
  }
  
  /**
   * Writes a Primary Volume Descriptor.
   *
   * @arg dst TStream to write data to.
   * @arg cdFormat Format of CD (e.g. CD-ROM or CD-XA).
   */
  void IsoPrimaryVolumeDescriptor::write(
      const Object* env, const Object* self, BlackT::TStream& dst,
      CdFormatIds::CdFormatId cdFormat) {
    dst.writeu8(self->getMemberInt("volumeDescriptorType"));
    std::string standardIdentifier
      = self->getMemberString("standardIdentifier");
    dst.write(standardIdentifier.c_str(), standardIdentifier.size());
    dst.writeu8(self->getMemberInt("volumeDescriptorVersion"));
    TStringConversion::writePaddedString(self->getMemberString("unused7"), dst, 1);
    TStringConversion::writePaddedString(self->getMemberString("systemIdentifier"), dst, 32, ' ');
    TStringConversion::writePaddedString(self->getMemberString("volumeIdentifier"), dst, 32, ' ');
    TStringConversion::writePaddedString(self->getMemberString("unused72"), dst, 8);
    dst.writeu32lebe(self->getMemberInt("volumeSpaceSize"));
    TStringConversion::writePaddedString(self->getMemberString("unused88"), dst, 32);
    dst.writeu16lebe(self->getMemberInt("volumeSetSize"));
    dst.writeu16lebe(self->getMemberInt("volumeSequenceNumber"));
    dst.writeu16lebe(self->getMemberInt("logicalBlockSize"));
    dst.writeu32lebe(self->getMemberInt("pathTableSize"));
    dst.writeu32le(self->getMemberInt("typeLPathTableOffset"));
    dst.writeu32le(self->getMemberInt("optionalTypeLPathTableOffset"));
    dst.writeu32be(self->getMemberInt("typeMPathTableOffset"));
    dst.writeu32be(self->getMemberInt("optionalTypeMPathTableOffset"));
    // fill this in later
    TStringConversion::writePaddedString(self->getMemberString("rootDirectoryRecord"), dst, 34);
    TStringConversion::writePaddedString(self->getMemberString("volumeSetIdentifier"), dst, 128, ' ');
    TStringConversion::writePaddedString(self->getMemberString("publisherIdentifier"), dst, 128, ' ');
    TStringConversion::writePaddedString(self->getMemberString("dataPreparerIdentifier"), dst, 128, ' ');
    TStringConversion::writePaddedString(self->getMemberString("applicationIdentifier"), dst, 128, ' ');
    TStringConversion::writePaddedString(self->getMemberString("copyrightFileIdentifier"), dst, 37, ' ');
    TStringConversion::writePaddedString(self->getMemberString("abstractFileIdentifier"), dst, 37, ' ');
    TStringConversion::writePaddedString(self->getMemberString("bibliographicFileIdentifier"), dst, 37, ' ');
/*    TStringConversion::writePaddedString(
      toPvdTimestring(self->getMemberInt("volumeCreationTime")), dst, 17);
    TStringConversion::writePaddedString(
      toPvdTimestring(self->getMemberInt("volumeModificationTime")), dst, 17);
    TStringConversion::writePaddedString(
      toPvdTimestring(self->getMemberInt("volumeExpirationTime")), dst, 17);
    TStringConversion::writePaddedString(
      toPvdTimestring(self->getMemberInt("volumeEffectiveTime")), dst, 17); */
    TStringConversion::writePaddedString(self->getMemberString("volumeCreationTime"), dst, 17);
    TStringConversion::writePaddedString(self->getMemberString("volumeModificationTime"), dst, 17);
    TStringConversion::writePaddedString(self->getMemberString("volumeExpirationTime"), dst, 17);
    TStringConversion::writePaddedString(self->getMemberString("volumeEffectiveTime"), dst, 17);
    dst.writeu8(self->getMemberInt("fileStructureVersion"));
    TStringConversion::writePaddedString(self->getMemberString("reserved882"), dst, 1);
    
    if (cdFormat == CdFormatIds::cdxa) {
      TStringConversion::writePaddedString(self->getMemberString("applicationUseXa883"), dst, 141);
      TStringConversion::writePaddedString(self->getMemberString("xa_signature"), dst, 8);
      dst.writeu16be(self->getMemberInt("xa_flags"));
      TStringConversion::writePaddedString(self->getMemberString("xa_startupDirectory"), dst, 8);
      TStringConversion::writePaddedString(self->getMemberString("xa_reserved1042"), dst, 8);
      TStringConversion::writePaddedString(self->getMemberString("applicationUseXa1050"), dst, 345);
    }
    else {
      TStringConversion::writePaddedString(self->getMemberString("applicationUseCdrom883"), dst, 512);
    }
    
    TStringConversion::writePaddedString(self->getMemberString("reserved1395"), dst, 653);
  }
  
  IsoPrimaryVolumeDescriptor::IsoPrimaryVolumeDescriptor()
    : BlobObject() {
    //============================
    // Set up functions
    //============================
    
//    addFunction("importDirectoryListing", importDirectoryListing);
    
    //============================
    // Set up members
    //============================
    
    setMember("volumeDescriptorType", CdConsts::primaryVolumeDescriptorTypeId);
    setMember("standardIdentifier", "CD001");
    setMember("volumeDescriptorVersion", 1);
    setMember("unused7", "");
    setMember("systemIdentifier", "");
    setMember("volumeIdentifier", "");
    setMember("unused72", "");
    setMember("volumeSpaceSize", 0);
    setMember("unused88", "");
    setMember("volumeSetSize", 1);
    setMember("volumeSequenceNumber", 1);
    setMember("logicalBlockSize", 0x800);
    setMember("pathTableSize", 0);
    setMember("typeLPathTableOffset", 0);
    setMember("optionalTypeLPathTableOffset", 0);
    setMember("typeMPathTableOffset", 0);
    setMember("optionalTypeMPathTableOffset", 0);
    // ?
//    setMember("rootDirectoryRecord", FileListing());
    setMember("rootDirectoryRecord", "");
    setMember("volumeSetIdentifier", "");
    setMember("publisherIdentifier", "");
    setMember("dataPreparerIdentifier", "");
    setMember("applicationIdentifier", "");
    setMember("copyrightFileIdentifier", "");
    setMember("abstractFileIdentifier", "");
    setMember("bibliographicFileIdentifier", "");
    setMember("volumeCreationTime", "");
    setMember("volumeModificationTime", "");
    setMember("volumeExpirationTime", "");
    setMember("volumeEffectiveTime", "");
    setMember("fileStructureVersion", 1);
    setMember("reserved882", "");
    // standard CD-ROM application use area.
    // if format is CD-XA, this is ignored.
    setMember("applicationUseCdrom883", "");
    
    // XA only
    setMember("applicationUseXa883", "");
    setMember("xa_signature", "CD-XA001");
    setMember("xa_flags", 0);
    setMember("xa_startupDirectory", "");
    setMember("xa_reserved1042", "");
    setMember("applicationUseXa1050", "");
    
    // remainder of data
    setMember("reserved1395", "");
  }


}
