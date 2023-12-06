#include "discaster/CdBuilder.h"
#include "discaster/CdConsts.h"
#include "discaster/CdMsf.h"
#include "discaster/Config.h"
#include "classes/FileListing.h"
#include "util/MiscMath.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TFileManip.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>

using namespace BlackT;

namespace Discaster {

  
  TrackEntry::TrackEntry()
    : startSector(0),
      sectorSize(0) { }
  
  const BlackT::TByte CdBuilder::sectorSyncField_[] = {
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00
  };
  
  CdBuilder::CdBuilder(std::iostream& cdbuf__)
    : 
//      outputFileName_(outputFileName__),
//      cdbuf(outputFileName__.c_str(),
//            std::ios_base::binary
//              | std::ios_base::trunc
//              | std::ios_base::in
//              | std::ios_base::out)
        cdbuf_(cdbuf__),
        discStreamBasePos_(cdbuf__.tellp()),
        writePos_(0),
        currentSectorNum_(0),
        dataAreaEndPos_(-1),
        currentTrackNum_(-1),
        currentSectorMode_(CdModeIds::mode1),
        maxSectorCount_(CdConsts::cdCapacityInSectors),
        initialPregapSectors_(CdConsts::numInitialPregapSectors),
        disableEccCalculation_(false),
        xaEorFlagMode_(XaSubheaderFlagModes::none),
        xaEofFlagMode_(XaSubheaderFlagModes::none),
        xaFileNumber_(0),
        xaChannelNumber_(0),
        xaVideoFlag_(false),
        xaAudioFlag_(false),
        xaDataFlag_(true),
        xaTriggerFlag_(false),
        xaRealTimeFlag_(false),
        newMode2SectorsAreEof_(false),
        newMode2SectorsAreEor_(false)
//        xaSubheaderIsAlwaysEor_(false),
//        xaSubheaderEndsInEor_(false),
//        xaSubheaderEndsInEof_(false)
    {
    // mark all sectors as unrecorded
    sectorModeMap_ = new BlackT::TByte[maxSectorCount_];
    std::memset(sectorModeMap_, 0xFF, maxSectorCount_);
    
    // prep blank sector array
    std::memset(blankSectorArray_, 0x00, sizeof(blankSectorArray_));
  }
  
  int CdBuilder::currentTrackNum() const {
    return currentTrackNum_;
  }
    
  int CdBuilder::currentSectorNum() const {
    return currentSectorNum_;
  }
  
  int CdBuilder::currentByteNum() const {
    if (sectorIsActive()) return writePos_;
    
    // if no sector active, assume that the next sector will be of the
    // same type as the previous and return start of its data area
    return sectorToBytePos(currentSectorNum_)
            + CdConsts::sectorDataAreaStartPos(currentSectorMode_);
  }
  
  void CdBuilder::startNewTrack(int trackNum, CdModeIds::CdModeId modeId) {
    // finish old track (if any)
    if (trackIsActive()) endCurrentTrack();
    
    currentTrackNum_ = trackNum;
    
    // record track start position
    trackIndex_[currentTrackNum_].startSector = currentSectorNum_;
    trackIndex_[currentTrackNum_].mode = modeId;
    
    // update default mode
    currentSectorMode_ = modeId;
  }
  
  void CdBuilder::endCurrentTrack() {
    // if no track active, error
/*    if (currentTrackNum_ == -1) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::endCurrentTrack()",
                              std::string(
                                "Tried to end nonexistent CD track"));
    } */
    
    // pad to next sector boundary
    padToSectorBoundary();
    
    // record track size
    TrackEntry& trackEntry = trackIndex_[currentTrackNum_];
    trackEntry.sectorSize = currentSectorNum_ - trackEntry.startSector;
    
    currentTrackNum_ = -1;
  }
  
  void CdBuilder::addTrackIndex(int indexNum) {
    // if no track active, error
    if (!trackIsActive()) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::startNewTrack()",
                              std::string(
                                "Tried to add index to nonexistent CD track"));
    }
    
    trackIndex_[currentTrackNum_].indices[indexNum] = currentSectorNum_;
  }
  
  void CdBuilder::writeData(const BlackT::TByte* data, int len) {
    if (len <= 0) return;
    
    // be lazy
    // we really should never have been dealing with raw arrays like this
    TBufStream ifs(len);
    ifs.write((char*)data, len);
    ifs.seek(0);
    
    writeData(ifs);
/*    if (xaEorFlagMode_ == XaSubheaderFlagModes::everySector)
      newMode2SectorsAreEor_ = true;
    if (xaEofFlagMode_ == XaSubheaderFlagModes::everySector)
      newMode2SectorsAreEof_ = true;
    
    int dataAreaStart = sectorDataAreaStartPos(currentSectorMode_);
    int dataAreaEnd = sectorDataAreaEndPos(currentSectorMode_);
    int dataAreaSize = dataAreaEnd - dataAreaStart;
    
    while (true) {
      // determine size of next transfer
//      int maxTransfer = dataAreaEndPos_ - writePos_;
      int sectorOffset = byteToSectorLocalPos(writePos_);
      int dataAreaOffset = sectorOffset - dataAreaStart;
      int maxTransfer = dataAreaSize - dataAreaOffset;
//      int transferSize = (len < maxTransfer) ? len : maxTransfer;
      int transferSize = maxTransfer;
      if (len < maxTransfer) {
        transferSize = len;
//        isLastTransfer = true;
        
        if (xaEorFlagMode_ == XaSubheaderFlagModes::lastSector)
          newMode2SectorsAreEor_ = true;
        if (xaEofFlagMode_ == XaSubheaderFlagModes::lastSector)
          newMode2SectorsAreEof_ = true;
      }
      
      // prep next sector if needed
      if (!sectorIsActive()) prepNextSector(currentSectorMode_);
      
      // copy source data to the data area
      cdbuf_.write((const char*)data, transferSize);
      
      // finish sector if filled
      writePos_ += transferSize;
      if (writePos_ >= dataAreaEndPos_) finishSector();
      
      // if all bytes transferred, done
      len -= transferSize;
      if (len <= 0) break;
      data += transferSize;
    }
    
    newMode2SectorsAreEof_ = false;
    newMode2SectorsAreEor_ = false; */
  }
  
  void CdBuilder::writeDataFromFile(std::string inputFileName) {
    if (!TFileManip::fileExists(inputFileName)) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::writeDataFromFile()",
                              std::string(
                                "Tried to write data from nonexistent file ")
                                + inputFileName);
    }
    
    TIfstream ifs(inputFileName.c_str(), std::ios_base::binary);
    writeData(ifs);
/*    int len = ifs.size();
    if (len <= 0) return;
    
    if (xaEorFlagMode_ == XaSubheaderFlagModes::everySector)
      newMode2SectorsAreEor_ = true;
    if (xaEofFlagMode_ == XaSubheaderFlagModes::everySector)
      newMode2SectorsAreEof_ = true;
    
    int dataAreaStart = sectorDataAreaStartPos(currentSectorMode_);
    int dataAreaEnd = sectorDataAreaEndPos(currentSectorMode_);
    int dataAreaSize = dataAreaEnd - dataAreaStart;
    
    char buffer[CdConsts::physicalSectorSize];
    while (true) {
      // determine size of next transfer
//      int maxTransfer = dataAreaEndPos_ - writePos_;
      int sectorOffset = byteToSectorLocalPos(writePos_);
      int dataAreaOffset = sectorOffset - dataAreaStart;
      int maxTransfer = dataAreaSize - dataAreaOffset;
//      int transferSize = (len < maxTransfer) ? len : maxTransfer;
      int transferSize = maxTransfer;
      if (len < maxTransfer) {
        transferSize = len;
//        isLastTransfer = true;
        
        if (xaEorFlagMode_ == XaSubheaderFlagModes::lastSector)
          newMode2SectorsAreEor_ = true;
        if (xaEofFlagMode_ == XaSubheaderFlagModes::lastSector)
          newMode2SectorsAreEof_ = true;
      }
      
      // prep next sector if needed
      if (!sectorIsActive()) prepNextSector(currentSectorMode_);
      
      // copy as many bytes as possible to the data area
//      int maxTransfer = dataAreaEndPos_ - writePos_;
//      int transferSize = (len < maxTransfer) ? len : maxTransfer;
      
      // read data from file
      ifs.read(buffer, transferSize);
      
      // write to output
      cdbuf_.write((const char*)buffer, transferSize);
      
      // finish sector if filled
      writePos_ += transferSize;
      if (writePos_ >= dataAreaEndPos_) finishSector();
      
      // if all bytes transferred, done
      len -= transferSize;
      if (len <= 0) break;
    }
    
    newMode2SectorsAreEof_ = false;
    newMode2SectorsAreEor_ = false; */
  }
  
  void CdBuilder::writeDataFromFileWithSkippedInitialSectors(
      std::string inputFileName, int skippedInitialSectors,
      int totalSize) {
    if (!TFileManip::fileExists(inputFileName)) {
      throw TGenericException(T_SRCANDLINE,
                    "CdBuilder::writeDataFromFileWithSkippedInitialSectors()",
                    std::string(
                      "Tried to write data from nonexistent file ")
                      + inputFileName);
    }
    
    TIfstream ifs(inputFileName.c_str(), std::ios_base::binary);
    ifs.seekoff(skippedInitialSectors * CdConsts::physicalSectorSize);
    if (totalSize == -1) {
      writeData(ifs);
    }
    else {
      TBufStream temp;
      temp.writeFrom(ifs, totalSize * CdConsts::physicalSectorSize);
      temp.seek(0);
      writeData(temp);
    }
  }
  
  void CdBuilder::writeDataFromFileWithXaFlags(std::string inputFileName,
                 XaSubheaderFlagModes::XaSubheaderFlagMode xaEorMode,
                 XaSubheaderFlagModes::XaSubheaderFlagMode xaEofMode) {
    if (!TFileManip::fileExists(inputFileName)) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::writeDataFromFileWithXaFlags()",
                              std::string(
                                "Tried to write data from nonexistent file ")
                                + inputFileName);
    }
    
    TIfstream ifs(inputFileName.c_str(), std::ios_base::binary);
    writeDataWithXaFlags(ifs, xaEorMode, xaEofMode);
  }
  
  void CdBuilder::writeData(BlackT::TStream& ifs) {
//    int len = ifs.size();
    int len = ifs.remaining();
    if (len <= 0) return;
    
    if (xaEorFlagMode_ == XaSubheaderFlagModes::everySector)
      newMode2SectorsAreEor_ = true;
    if (xaEofFlagMode_ == XaSubheaderFlagModes::everySector)
      newMode2SectorsAreEof_ = true;
    
    int dataAreaStart = CdConsts::sectorDataAreaStartPos(currentSectorMode_);
    int dataAreaEnd = CdConsts::sectorDataAreaEndPos(currentSectorMode_);
    int dataAreaSize = CdConsts::sectorDataAreaSize(currentSectorMode_);
    
//    std::cerr << "start: " << dataAreaStart
//      << ", end: " << dataAreaEnd
//      << ", size: " << dataAreaSize << std::endl;
    
    char buffer[CdConsts::physicalSectorSize];
    while (true) {
      // determine size of next transfer
//      int maxTransfer = dataAreaEndPos_ - writePos_;
      int sectorOffset;
      if (!sectorIsActive()) sectorOffset = dataAreaStart;
      else sectorOffset = byteToSectorLocalPos(writePos_);
      int dataAreaOffset = sectorOffset - dataAreaStart;
      int maxTransfer = dataAreaSize - dataAreaOffset;
//      int transferSize = (len < maxTransfer) ? len : maxTransfer;
      int transferSize = maxTransfer;
//      std::cerr << "sectorOffset: " << sectorOffset
//        << ", dataAreaOffset: " << dataAreaOffset
//        << ", maxTransfer: " << maxTransfer
//        << ", transferSize: " << transferSize << std::endl;
      if (len < maxTransfer) {
        transferSize = len;
//        isLastTransfer = true;
      }
      
      if (len <= maxTransfer) {
        if (xaEorFlagMode_ == XaSubheaderFlagModes::lastSector)
          newMode2SectorsAreEor_ = true;
        if (xaEofFlagMode_ == XaSubheaderFlagModes::lastSector)
          newMode2SectorsAreEof_ = true;
      }
      
      // prep next sector if needed
      if (!sectorIsActive()) prepNextSector(currentSectorMode_);
      
      // copy as many bytes as possible to the data area
//      int maxTransfer = dataAreaEndPos_ - writePos_;
//      int transferSize = (len < maxTransfer) ? len : maxTransfer;
      
      // read data from file
      ifs.read(buffer, transferSize);
      
      // write to output
      cdbuf_.write((const char*)buffer, transferSize);
      
      // finish sector if filled
      writePos_ += transferSize;
      if (writePos_ >= dataAreaEndPos_) finishSector();
      
      // if all bytes transferred, done
      len -= transferSize;
      if (len <= 0) break;
    }
    
    newMode2SectorsAreEof_ = false;
    newMode2SectorsAreEor_ = false;
  }
  
  void CdBuilder::writeRawSectorData(BlackT::TStream& ifs) {
    if (sectorIsActive()) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::writeRawSectorData()",
                              std::string("Tried to write raw sector ")
                                + "data without sector alignment");
    }
    
//    int len = ifs.size();
    int len = ifs.remaining();
    if (len <= 0) return;
    
    if ((len % CdConsts::physicalSectorSize) != 0) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::writeRawSectorData()",
                              std::string("Tried to write raw sector ")
                                + "data with length not divisible by"
                                + TStringConversion
                                    ::toString(CdConsts::physicalSectorSize));
    }
    
    // compute number of sectors required for data
    int numSectors = len / CdConsts::physicalSectorSize;
//    if ((len % CdConsts::physicalSectorSize) != 0) ++numSectors;

    // ensure enough sectors are left to hold the data
    if ((currentSectorNum_ + numSectors) > maxSectorCount_) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::writeRawSectorData()",
                              std::string("End of disc exceeded: ")
                                + "need "
                                + TStringConversion
                                    ::toString(numSectors)
                                 + " sectors, "
                                + TStringConversion
                                    ::toString(maxSectorCount_
                                                - currentSectorNum_)
                                 + " left");
    }
    
    // save current sector mode so we can restore it afterward
    CdModeIds::CdModeId oldSectorMode = currentSectorMode_;
    
    // copy sectors from source to output stream
    for (int i = 0; i < numSectors; i++) {
//      int basePos = writePos_;
      unsigned int oldStreamPos = cdbuf_.tellp();
      
      char buffer[CdConsts::physicalSectorSize];
      ifs.read(buffer, CdConsts::physicalSectorSize);
      cdbuf_.write(buffer, CdConsts::physicalSectorSize);
      
      unsigned int newStreamPos = cdbuf_.tellp();
      
//          std::string modestring = fileListingObj.getMemberString(
//            "rawSectorModestring");
//          currentSectorMode_ = CdConsts::modeStringToId(modestring);
      // determine mode of this sector
      currentSectorMode_ = CdConsts::modeDataToId(
        buffer[CdConsts::sectorHeaderModeOffset],
        buffer[CdConsts::mode2SubheaderSubmodeOffset]);
      
      // mark sector as formatted in the current mode
      sectorModeMap_[currentSectorNum_] = (TByte)currentSectorMode_;
      
      // write correct msf information if applicable to current mode
      if ((currentSectorMode_ == CdModeIds::mode0)
          || (currentSectorMode_ == CdModeIds::mode1)
          || (currentSectorMode_ == CdModeIds::mode2form1)
          || (currentSectorMode_ == CdModeIds::mode2form2)
          ) {
        cdbuf_.seekp(oldStreamPos + CdConsts::sectorHeaderStartOffset);
        
        int discAbsoluteSectorNum = currentSectorNum_ + initialPregapSectors_;
        CdMsf msf;
        msf.fromSectorNumToBcd(discAbsoluteSectorNum);
        cdbuf_.put((char)((unsigned char)msf.min));
        cdbuf_.put((char)((unsigned char)msf.sec));
        cdbuf_.put((char)((unsigned char)msf.frame));
      }
      
      // seek to end of new sector
      ++currentSectorNum_;
      writePos_ += CdConsts::physicalSectorSize;
      cdbuf_.seekp(newStreamPos);
    }
    
    // restore original sector mode
    currentSectorMode_ = oldSectorMode;
  }
  
  void CdBuilder::writeDataWithXaFlags(BlackT::TStream& ifs,
                 XaSubheaderFlagModes::XaSubheaderFlagMode xaEorMode,
                 XaSubheaderFlagModes::XaSubheaderFlagMode xaEofMode) {
    XaSubheaderFlagModes::XaSubheaderFlagMode oldEorMode
      = xaEorFlagMode_;
    XaSubheaderFlagModes::XaSubheaderFlagMode oldEofMode
      = xaEofFlagMode_;
    
    xaEorFlagMode_ = xaEorMode;
    xaEofFlagMode_ = xaEofMode;
    
    writeData(ifs);
    
    xaEorFlagMode_ = oldEorMode;
    xaEofFlagMode_ = oldEofMode;
  }
  
  void CdBuilder::overwriteData(int offset, const BlackT::TByte* src, int len) {
    if ((offset + len) >= writePos_) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::overwriteData()",
                              std::string(
                                "Overwriting past end of disc: max ")
                                + TStringConversion::toString(writePos_)
                                + ", targeted "
                                + TStringConversion::toString(offset)
                                + " with "
                                + TStringConversion::toString(len)
                                + " bytes of data");
    }
    
    beginOverwrite(offset);
      // do a normal write
      writeData(src, len);
    endOverwrite();
  }
  
  void CdBuilder::beginOverwrite(int offset) {
    // save current write state
    old_currentSectorMode_ = currentSectorMode_;
    old_writePos_ = writePos_;
    old_currentSectorNum_ = currentSectorNum_;
    old_dataAreaEndPos_ = dataAreaEndPos_;
    old_cdbufPutPos_ = cdbuf_.tellp();
    
    // compute target sector number
    int targetStartSector = byteToSectorPos(offset);
    // compute target byte number within sector
    int targetStartByteSectorLocal = byteToSectorLocalPos(offset);
    
    // get target sector mode
    CdModeIds::CdModeId modeId = getSectorMode(targetStartSector);
    currentSectorMode_ = modeId;
    
    // compute where target offset falls in the data area of the target mode
    int dataAreaStart = CdConsts::sectorDataAreaStartPos(modeId);
    int dataAreaEnd = CdConsts::sectorDataAreaEndPos(modeId);
    int offsetFromDataAreaStart = targetStartByteSectorLocal - dataAreaStart;
    
    // make sure this is a valid position within the data area
    if ((targetStartByteSectorLocal < dataAreaStart)
        || (targetStartByteSectorLocal >= dataAreaEnd)) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::beginOverwrite()",
                              std::string(
                                "Overwrite begins outside of Data Area: ")
                                + "targeted sector "
                                + TStringConversion::toString(targetStartSector)
                                + " at "
                                + TStringConversion::toString(targetStartByteSectorLocal)
                                + " (area start: "
                                + TStringConversion::toString(dataAreaStart)
                                + ", area end: "
                                + TStringConversion::toString(dataAreaEnd)
                                + ", mode ID: "
                                + TStringConversion::toString((int)modeId)
                                + ")");
    }
    
    // simulate a sector prep at the current sector.
    // nothing will actually be overwritten because the sector is already
    // marked as formatted, but the write position will be updated
    // to the start of the sector's data area.
    dataAreaEndPos_ = -1;
    currentSectorNum_ = targetStartSector;
    prepNextSector(modeId);
    
    // seek to the target sector position
    writePos_ += offsetFromDataAreaStart;
    cdbuf_.seekp(discStreamBasePos_ + writePos_);
  }
  
  void CdBuilder::beginOverwriteAtSector(int sectorNum) {
    // get target sector mode
    CdModeIds::CdModeId modeId = getSectorMode(sectorNum);
    // byte offset for start of sector
    int offset = sectorToBytePos(sectorNum);
    // compute start of data area
    int dataAreaStart = CdConsts::sectorDataAreaStartPos(modeId);
    offset += dataAreaStart;
    
    // do normal setup for start of data area
    beginOverwrite(offset);
    
    // force sector to be inactive (so user can have pad commands after this
    // call that work as expected)
    dataAreaEndPos_ = -1;
//    writePos_ -= dataAreaStart;
//    cdbuf_.seekp(discStreamBasePos_ + writePos_);
  }
  
  void CdBuilder::endOverwrite() {
    // restore old settings
    currentSectorMode_ = old_currentSectorMode_;
    writePos_ = old_writePos_;
    currentSectorNum_ = old_currentSectorNum_;
    dataAreaEndPos_ = old_dataAreaEndPos_;
    cdbuf_.seekp(old_cdbufPutPos_);
  }
    
  void CdBuilder::resolveDiscReferencesStep(
      Object* env, Object* iso, Object* directoryListing) {
    
    Object* directoryMap
      = FileListing::getDirectoryMap(env, directoryListing);
        
    if (config.debugOutput()) {
      std::cout << "Resolving disc references for directory "
        << "\""
        << directoryListing->getMemberString("name")
        << "\""
        << std::endl;
    }
    
    // pass 1: resolve remaining offsets
    for (ObjectMemberMap::iterator it = directoryMap->members().begin();
         it != directoryMap->members().end();
         ++it) {
      Object& fileObj = it->second;
      
      if (fileObj.getMemberInt("isParentReference") != 0) {
//        Object& parentObj
//          = *IsoFilesystem::getDirectoryListingObjectFromPath(env, iso,
//                fileObj.getMemberString("parentPath"));
        Object& parentObj
          = *IsoFilesystem::getDirectoryListingObjectFromPath(env, iso,
                directoryListing->getMemberString("parentPath"));
//        std::cerr << "parent: " << parentObj.getMemberString("name")
//          << " , name: " << fileObj.getMemberString("name") << std::endl;
        fileObj.setMember("dataOffset",
                          parentObj.getMember("dataOffset"));
        fileObj.setMember("dataSize",
                          parentObj.getMember("dataSize"));
      }
      else if (fileObj.getMemberInt("isSelfReference") != 0) {
        // already done
      }
      // update disc pointers using labels
      else if (fileObj.type().compare("DiscPointerListing") == 0) {
        std::string name = fileObj.getMemberString("name");
        
        std::string startLabelName = fileObj.getMemberString("startLabelName");
        std::string endLabelName = fileObj.getMemberString("endLabelName");
        
        if (config.debugOutput()) {
          std::cout << "Resolving disc pointer "
            << "\""
            << fileObj.getMemberString("name")
            << "\""
            << " between labels "
            << "\""
            << startLabelName
            << "\""
            << " and "
            << "\""
            << endLabelName
            << "\""
            << std::endl;
        }
        
        if (!discLabels_.hasMember(startLabelName)) {
          throw TGenericException(T_SRCANDLINE,
                                  "resolveDiscReferencesStep()",
                                  std::string("Disc pointer ")
                                    + "\""
                                    + name
                                    + "\""
                                    + " points to undefined start label "
                                    + "\""
                                    + startLabelName
                                    + "\"");
        }
        else if (!discLabels_.hasMember(endLabelName)) {
          throw TGenericException(T_SRCANDLINE,
                                  "resolveDiscReferencesStep()",
                                  std::string("Disc pointer ")
                                    + "\""
                                    + name
                                    + "\""
                                    + " points to undefined end label "
                                    + "\""
                                    + endLabelName
                                    + "\"");
        }
        
        int startSectorNum = discLabels_.getMemberInt(startLabelName);
        int endSectorNum = discLabels_.getMemberInt(endLabelName);
        int sizeInSectors = endSectorNum - startSectorNum;
        int sizeInBytes = sectorToBytePos(sizeInSectors);
        
        if (config.debugOutput()) {
          std::cout << "Resolved disc pointer "
            << "\""
            << fileObj.getMemberString("name")
            << "\""
            << " to disc sector "
            << startSectorNum
            << " (length: "
            << sizeInSectors
            << " sectors)"
            << std::endl;
        }
        
        int logicalBlockOffset
          = getLogicalBlockOffset(env, iso, startSectorNum);
//        int sizeInLogicalBlocks
//          = getLogicalBlockSize(env, iso, sizeInSectors);
        
        if ((logicalBlockOffset < 0)
            && (config.warningsOn())) {
          std::cerr << "WARNING: "
            << "disc pointer " << name
            << " has negative logical block offset to label "
            << startLabelName
            << std::endl;
        }
        
        fileObj.getMember("dataOffset").setIntValue(logicalBlockOffset);
//        fileObj.getMember("dataSize").setIntValue(sizeInLogicalBlocks);
        fileObj.getMember("dataSize").setIntValue(sizeInBytes);
      }
      // recursively handle subdirectories
      else if (fileObj.getMemberInt("isDirectory") != 0) {
        resolveDiscReferencesStep(env, iso, &fileObj);
      }
      
    }
  }
    
  void CdBuilder::rewriteDiscReferencesStep(
      Object* env, Object* iso, Object* directoryListing,
      std::string path) {
    
    Object* directoryMap
      = FileListing::getDirectoryMap(env, directoryListing);
    
    std::string cdFormatString = iso->getMemberString("format");
    CdFormatIds::CdFormatId cdFormatId
      = CdConsts::formatStringToId(cdFormatString);
      
    // pass 2: write updated descriptor for this directory
    
    if (!directoryListing->hasMember("build_written")) {
      if (config.warningsOn()) {
        std::cerr << "WARNING: "
          << "No descriptor for directory \"" << path
          << "\" placed on disc"
          << std::endl;
      }
    }
    else {
      int directoryDescriptorStartSector
        = directoryListing->getMemberInt("build_absSectorNum");
      
      if (config.debugOutput()) {
        std::cout << "Rewriting finalized directory descriptor for "
          << "\""
          << directoryListing->getMemberString("name")
          << "\""
          << " at sector "
          << directoryDescriptorStartSector
          << std::endl;
      }
      
      beginOverwriteAtSector(directoryDescriptorStartSector);
      
  //     TBufStream dst;
        writeDirectoryDescriptor(*env, *iso, directoryListing, path, cdFormatId);
        
  //        writeData((BlackT::TByte*)dst.data().data(), dst.size());
      endOverwrite();
    }
    
    // repeat for subdirectories
    for (ObjectMemberMap::iterator it = directoryMap->members().begin();
         it != directoryMap->members().end();
         ++it) {
      Object& fileObj = it->second;
      
      if (fileObj.getMemberInt("isDirectory") == 0) {
        // make sure this is an actual file and not a disc pointer
        if ((fileObj.type().compare("FileListing") == 0)
              && !fileObj.hasMember("build_absSectorNum")) {
          if (config.warningsOn()) {
            std::cerr << "WARNING: "
              << "File \"" << path + "/" + fileObj.getMemberString("name")
              << "\" present in file listing, but not placed on disc"
              << std::endl;
          }
        }
        
      }
      else {
//        if ((fileObj.getMemberInt("isDirectory") != 0)
        if ((fileObj.getMemberInt("isSelfReference") == 0)
            && (fileObj.getMemberInt("isParentReference") == 0)) {
          // recursively handle subdirectories
          rewriteDiscReferencesStep(env, iso, &fileObj,
              path + "/" + fileObj.getMemberString("name"));
        }
      }
      
    }
  }
  
  void CdBuilder::addDiscLabel(std::string name) {
    int sectorNum = currentSectorNum();
    
    // if used while sector active, label applies to the *next*
    // sector. otherwise, labels will incorrectly point to the last
    // sector of the previous content instead of the start of the new.
    if (sectorIsActive()) ++sectorNum;
    
    if (config.debugOutput()) {
      std::cout << "Sector " << currentSectorNum() << ": "
                << "Adding disc label \"" << name << "\"" << std::endl;
    }
    
    discLabels_.setMember(name, sectorNum);
  }
  
  void CdBuilder::addPregap(int sizeInSectors) {
    // error if not part of a track
    if (!trackIsActive()) {
      throw TGenericException(T_SRCANDLINE,
                              "addPregap()",
                              std::string("Tried to create a pregap not part ")
                                + "of any track");
    }
    
    TrackEntry& trackInfo = trackIndex_.at(currentTrackNum_);
    
    // make sure track indices 0/1 are not already defined
    
    if (trackInfo.indices.find(0) != trackInfo.indices.end()) {
      throw TGenericException(T_SRCANDLINE,
                              "addPregap()",
                              std::string("Tried to add pregap to track ")
                                + TStringConversion::toString(currentTrackNum_)
                                + ", which already has index 0 defined");
    }
    
    if (trackInfo.indices.find(1) != trackInfo.indices.end()) {
      throw TGenericException(T_SRCANDLINE,
                              "addPregap()",
                              std::string("Tried to add pregap to track ")
                                + TStringConversion::toString(currentTrackNum_)
                                + ", which already has index 1 defined");
    }
  
    // add indices and insert null sectors
    padToSectorBoundary();
    addTrackIndex(0);
    addNullSectors(sizeInSectors);
    addTrackIndex(1);
    
  }
  
  void CdBuilder::changeSectorMode(CdModeIds::CdModeId modeId) {
    padToSectorBoundary();
    currentSectorMode_ = modeId;
  }
  
  void CdBuilder::setDisableEccCalculation(bool disableEccCalculation__) {
    disableEccCalculation_ = disableEccCalculation__;
  }
  
  void CdBuilder::finishCd(Object* env) {
    // close current sector if active
    if (sectorIsActive()) finishSector();
    
    //==========================================================================
    // Update ISOs
    //==========================================================================
    
    for (IsoWithAssociatesCollection::iterator it = isosWithAssociates_.begin();
         it != isosWithAssociates_.end();
         ++it) {
      IsoWithAssociates& isoWithAssociates = *it;
      Object& iso = isoWithAssociates.iso;
      
      //======================================
      // update parent directory records
      // with final size/location of
      // descriptors.
      // also handles disc labels
      //======================================
      
      resolveDiscReferencesStep(
          env, &iso, &(iso.getMember("directoryListing")));
      
      //======================================
      // update path tables
      //======================================
      
      updatePathTable(*env, iso, isoWithAssociates.typeLPathTable,
                      EndiannessTypes::little);
      updatePathTable(*env, iso, isoWithAssociates.typeLPathTableCopy,
                      EndiannessTypes::little);
      updatePathTable(*env, iso, isoWithAssociates.typeMPathTable,
                      EndiannessTypes::big);
      updatePathTable(*env, iso, isoWithAssociates.typeMPathTableCopy,
                      EndiannessTypes::big);
      
      //======================================
      // overwrite all directory records with
      // finalized data
      //======================================
      
      rewriteDiscReferencesStep(
          env, &iso, &(iso.getMember("directoryListing")), "");
      
      //======================================
      // update primary volume descriptors
      // with final volume space size,
      // path table size, and root directory
      // record
      //======================================
      
      std::string cdFormatString = iso.getMemberString("format");
      CdFormatIds::CdFormatId cdFormatId
        = CdConsts::formatStringToId(cdFormatString);
      
      int isoSize = iso.getMemberInt("build_totalSectorSize");
      
      Object& rootDirRecord = iso.getMember("directoryListing");
      
      // write root directory record and convert to string
      TBufStream rootDirRecordStream;
      // format must be standard CD-ROM -- hard limit of 34 bytes
      FileListing::writeDirectoryRecord(env, &rootDirRecord,
        rootDirRecordStream, CdFormatIds::cdrom);
      DiscasterString rootDirRecordStr;
      rootDirRecordStr.assign(rootDirRecordStream.data().data(),
                              rootDirRecordStream.size());
      
      for (IsoPrimaryVolumeDescriptorCollection::iterator jt
             = it->primaryVolumeDescriptors.begin();
           jt != it->primaryVolumeDescriptors.end();
           ++jt) {
        Object& primaryVolumeDescriptor = *jt;
        
        // update members
//        primaryVolumeDescriptor.getMember("volumeSpaceSize")
//          .setIntValue(getLogicalBlockSize(env, &iso, isoSize));
        // volume space size is actually the size of the entire disc
        // in logical blocks, not just the target iso
        // FIXME: hardcoded logical block size of 0x800
        primaryVolumeDescriptor.getMember("volumeSpaceSize")
          .setIntValue(currentSectorNum_);
        primaryVolumeDescriptor.getMember("rootDirectoryRecord")
          .setStringValue(rootDirRecordStr);
        
        // if any path table exists, provide its size
        if (isoWithAssociates.typeLPathTable.hasMember("build_size"))
          primaryVolumeDescriptor.getMember("pathTableSize").setIntValue(
            isoWithAssociates.typeLPathTable.getMemberInt("build_size"));
        else if (isoWithAssociates.typeLPathTableCopy.hasMember("build_size"))
          primaryVolumeDescriptor.getMember("pathTableSize").setIntValue(
            isoWithAssociates.typeLPathTableCopy.getMemberInt("build_size"));
        else if (isoWithAssociates.typeMPathTable.hasMember("build_size"))
          primaryVolumeDescriptor.getMember("pathTableSize").setIntValue(
            isoWithAssociates.typeMPathTable.getMemberInt("build_size"));
        else if (isoWithAssociates.typeMPathTableCopy.hasMember("build_size"))
          primaryVolumeDescriptor.getMember("pathTableSize").setIntValue(
            isoWithAssociates.typeMPathTableCopy.getMemberInt("build_size"));
        
        // add extant path table offsets
        if (isoWithAssociates.typeLPathTable.hasMember("build_absSectorNum")) {
          primaryVolumeDescriptor.getMember("typeLPathTableOffset")
            .setIntValue(getLogicalBlockOffset(env, &iso,
              isoWithAssociates.typeLPathTable.getMemberInt("build_absSectorNum")));
        }
        if (isoWithAssociates.typeLPathTableCopy.hasMember("build_absSectorNum")) {
          primaryVolumeDescriptor.getMember("optionalTypeLPathTableOffset")
            .setIntValue(getLogicalBlockOffset(env, &iso,
              isoWithAssociates.typeLPathTableCopy.getMemberInt("build_absSectorNum")));
        }
        if (isoWithAssociates.typeMPathTable.hasMember("build_absSectorNum")) {
          primaryVolumeDescriptor.getMember("typeMPathTableOffset")
            .setIntValue(getLogicalBlockOffset(env, &iso,
              isoWithAssociates.typeMPathTable.getMemberInt("build_absSectorNum")));
        }
        if (isoWithAssociates.typeMPathTableCopy.hasMember("build_absSectorNum")) {
          primaryVolumeDescriptor.getMember("optionalTypeMPathTableOffset")
            .setIntValue(getLogicalBlockOffset(env, &iso,
              isoWithAssociates.typeMPathTableCopy.getMemberInt("build_absSectorNum")));
        }
        
        // overwrite updated table
        int pvdStartSector
          = primaryVolumeDescriptor.getMemberInt("build_absSectorNum");
        
        beginOverwriteAtSector(pvdStartSector);
          if (config.debugOutput()) {
            std::cout << "Sector " << currentSectorNum() << ": "
              << "Rewriting primary volume descriptor with finalized values"
              << std::endl;
          }
          
          TBufStream ofs;
          IsoPrimaryVolumeDescriptor::write(env, &primaryVolumeDescriptor,
            ofs, cdFormatId);
          ofs.seek(0);
          
//          writeData((TByte*)ofs.data().data(), ofs.size());
          writeDataWithXaFlags(ofs,
                               XaSubheaderFlagModes::lastSector,
                               XaSubheaderFlagModes::lastSector);
        endOverwrite();
      }
    }
    
    //==========================================================================
    // add ECC and EDC to sectors that need
    // it
    //==========================================================================
    
    if (config.debugOutput()) {
//      if (disableEccEdcCalculation_) {
//        std::cout
//          << "Skipping disc ECC/EDC calculations"
//          << std::endl;
//      }
//      else {
        std::cout
          << "Evaluating disc ECC/EDC"
          << std::endl;
//      }
      if (disableEccCalculation_) {
        std::cout << "Note: skipping ECC calculations" << std::endl;
      }
    }
    
    // create Galois field and generator for CIRC calculations
    
    // 0x11D = primitive polynomial for ecc:
    // x^8 + x^4 + x^3 + x^2 + 1
    // 2 = primitive element
    TGaloisField256Cd field;
    // the generator polynomial as defined in the cd specification
//    TGaloisField256Polynomial generator =
//      field.multPoly(TGaloisField256Polynomial({1, 1}),
//                     TGaloisField256Polynomial({1, 2}));
    
    // Fill in any necessary EDC/ECC fields for each sector
    for (int i = 0; i < currentSectorNum_; i++) {
      if (sectorModeMap_[i] == 0xFF) {
        throw TGenericException(T_SRCANDLINE,
                                "CdBuilder::finishCd()",
                                std::string(
                                  "Disc contains unformatted sector: ")
                                  + TStringConversion::toString(i));
      }
      
      CdModeIds::CdModeId modeId
        = static_cast<CdModeIds::CdModeId>((int)sectorModeMap_[i]);
        
      // check if any updates to sector needed
      switch (modeId) {
      // these modes have no additional fields to fill in
      case CdModeIds::audio:
      case CdModeIds::mode0:
      case CdModeIds::mode1raw:
        continue;
        break;
      default:
        // otherwise, proceed
        break;
      }
      
      int bytePos = sectorToBytePos(i);
      
      // read sector to memory for the calculations
      cdbuf_.seekg((unsigned int)(discStreamBasePos_ + bytePos));
      BlackT::TByte sectorBuffer[CdConsts::physicalSectorSize];
      cdbuf_.read((char*)sectorBuffer, CdConsts::physicalSectorSize);
      
      // for mode2 data, make subheader copy
      if ((modeId == CdModeIds::mode2form1)
          || (modeId == CdModeIds::mode2form2)) {
        cdbuf_.seekp((unsigned int)(discStreamBasePos_ + bytePos
                       + CdConsts::mode2SubheaderCopyStartOffset));
        // copy to pending image
        cdbuf_.write((char*)sectorBuffer + CdConsts::mode2SubheaderStartOffset,
                     CdConsts::mode2SubheaderEndOffset
                      - CdConsts::mode2SubheaderStartOffset);
        // copy to local buffer
        std::memcpy((char*)sectorBuffer
                      + CdConsts::mode2SubheaderCopyStartOffset,
                    (char*)sectorBuffer
                      + CdConsts::mode2SubheaderStartOffset,
                    CdConsts::mode2SubheaderEndOffset
                      - CdConsts::mode2SubheaderStartOffset);
      }
      
      // continue here rather than skipping this part of the code
      // entirely so that mode2 sectors will be correctly filled
//      if (disableEccEdcCalculation_) continue;
      
      switch (modeId) {
      // 0x810-0x813 = EDC over 0x0-0x80F
      // 0x81C-0x92F = ECC
      case CdModeIds::mode1:
      {
        // compute EDC
        unsigned int edc = computeEdc(
          sectorBuffer
            + CdConsts::mode1EdcCheckedDataStartOffset,
          CdConsts::mode1EdcCheckedDataEndOffset
            - CdConsts::mode1EdcCheckedDataStartOffset);
        
        // update sector buffer with EDC value
        ByteConversion::toBytes(edc,
                                (char*)sectorBuffer
                                  + CdConsts::mode1EdcStartOffset,
                                ByteSizes::uint32Size,
                                EndiannessTypes::little,
                                SignednessTypes::nosign);
        
        // write EDC to sector on disk
        cdbuf_.seekp((unsigned int)(discStreamBasePos_ + bytePos
                                    + CdConsts::mode1EdcStartOffset));
        cdbuf_.write((char*)sectorBuffer + CdConsts::mode1EdcStartOffset,
                     ByteSizes::uint32Size);
        
        if (disableEccCalculation_) continue;
        
        // compute ECC
        computeSectorEcc(sectorBuffer + CdConsts::mode1EccCheckedDataStartOffset,
                         sectorBuffer + CdConsts::mode1EccStartOffset,
                         field
//                         generator
                         );
        
        // write ECC data
        cdbuf_.seekp((unsigned int)(discStreamBasePos_ + bytePos
                                      + CdConsts::mode1EccStartOffset));
        cdbuf_.write(
          (char*)sectorBuffer + CdConsts::mode1EccStartOffset,
          CdConsts::mode1EccEndOffset - CdConsts::mode1EccStartOffset);
      }
        break;
      // 0x818-0x81B = EDC over 0x10-0x817
      // 0x81C-0x92F = ECC
      case CdModeIds::mode2form1:
      {
        // compute EDC
        unsigned int edc = computeEdc(
          sectorBuffer
            + CdConsts::mode2Form1EdcCheckedDataStartOffset,
          CdConsts::mode2Form1EdcCheckedDataEndOffset
            - CdConsts::mode2Form1EdcCheckedDataStartOffset);
        
        // update sector buffer with EDC value
        ByteConversion::toBytes(edc,
                                (char*)sectorBuffer
                                  + CdConsts::mode2Form1EdcStartOffset,
                                ByteSizes::uint32Size,
                                EndiannessTypes::little,
                                SignednessTypes::nosign);
        
        // write EDC to sector on disk
        cdbuf_.seekp((unsigned int)(discStreamBasePos_ + bytePos
                                    + CdConsts::mode2Form1EdcStartOffset));
        cdbuf_.write((char*)sectorBuffer + CdConsts::mode2Form1EdcStartOffset,
                     ByteSizes::uint32Size);
        
        if (disableEccCalculation_) continue;
        
        // zero-fill header. for some reason, it must be treated as zero for
        // this mode.
        {
          char emptybuf[] = { 0, 0, 0, 0 };
          std::memcpy((char*)sectorBuffer + CdConsts::sectorHeaderStartOffset,
                      emptybuf,
                      CdConsts::sectorHeaderEndOffset
                        - CdConsts::sectorHeaderStartOffset);
                        
                        
//          std::cerr << "HERE" << std::endl;
//          std::memcpy((char*)sectorBuffer + CdConsts::sectorHeaderStartOffset + 4,
//                      emptybuf,
//                      CdConsts::sectorHeaderEndOffset
//                        - CdConsts::sectorHeaderStartOffset);
        }
        
        // compute ECC
        computeSectorEcc(sectorBuffer + CdConsts::mode2Form1EccCheckedDataStartOffset,
                         sectorBuffer + CdConsts::mode2Form1EccStartOffset,
                         field
//                         generator
                         );
        
        // write ECC data
        cdbuf_.seekp((unsigned int)(discStreamBasePos_ + bytePos
                                      + CdConsts::mode2Form1EccStartOffset));
        cdbuf_.write(
          (char*)sectorBuffer + CdConsts::mode2Form1EccStartOffset,
          CdConsts::mode2Form1EccEndOffset - CdConsts::mode2Form1EccStartOffset);
        
      }
        break;
      // 0x92C-0x92F = EDC over 0x10-0x92B
      case CdModeIds::mode2form2:
      {
        // TODO: EDC is optional and user should be able to disable it.
        //       in this case, the EDC should be zero-filled, which we
        //       have already done and so need not do anything.
        
        // compute EDC
        unsigned int edc = computeEdc(
          sectorBuffer
            + CdConsts::mode2Form2EdcCheckedDataStartOffset,
          CdConsts::mode2Form2EdcCheckedDataEndOffset
            - CdConsts::mode2Form2EdcCheckedDataStartOffset);
        
        // update sector buffer with EDC value
        ByteConversion::toBytes(edc,
                                (char*)sectorBuffer
                                  + CdConsts::mode2Form2EdcStartOffset,
                                ByteSizes::uint32Size,
                                EndiannessTypes::little,
                                SignednessTypes::nosign);
        
        // write EDC to sector on disk
        cdbuf_.seekp((unsigned int)(discStreamBasePos_ + bytePos
                                    + CdConsts::mode2Form2EdcStartOffset));
        cdbuf_.write((char*)sectorBuffer + CdConsts::mode2Form2EdcStartOffset,
                     ByteSizes::uint32Size);
        
      }  
        break;
      // no additional fields
//      case CdModeIds::audio:
//      case CdModeIds::mode0:
//      case CdModeIds::mode1raw:
      default:
        
        break;
      }
      
      // for mode2 data, make subheader copy
/*      if ((modeId == CdModeIds::mode2form1)
          || (modeId == CdModeIds::mode2form2)) {
        cdbuf_.seekp((unsigned int)(discStreamBasePos_ + bytePos
                       + CdConsts::mode2SubheaderCopyStartOffset));
        cdbuf_.write((char*)sectorBuffer + CdConsts::mode2SubheaderStartOffset,
                     CdConsts::mode2SubheaderEndOffset
                      - CdConsts::mode2SubheaderStartOffset);
      } */
    }
    
    if (config.debugOutput()) {
      std::cout << "CD finished" << std::endl;
    }
  }
  
  void CdBuilder::exportCueSheet(std::string exportCueName,
                        std::string exportBinName) const {
    // make sure directory to put file in exists
    TFileManip::createDirectoryForFile(exportCueName);
    
    // the BIN name only, no directory part
    std::string baseFile = TFileManip::getFilename(exportBinName);
    
    if (config.debugOutput()) {
      std::cout << "Exporting CUE sheet to "
        << "\""
        << exportCueName
        << "\""
        << std::endl;
    }
    
    std::ofstream ofs(exportCueName.c_str(), std::ios_base::trunc);
    
    ofs << "FILE \"" << baseFile << "\" BINARY" << std::endl;
    for (TrackSectorMap::const_iterator it = trackIndex_.cbegin();
         it != trackIndex_.cend();
         ++it) {
      int trackNum = it->first;
      const TrackEntry& entry = it->second;
      
      ofs << "  TRACK "
          << std::setfill('0') << std::setw(2) << trackNum;
      
      std::string modename = CdConsts::idToCueStyleModeString(entry.mode);
      ofs << " " << modename << std::endl;
      
      for (IndexSectorMap::const_iterator jt = entry.indices.cbegin();
           jt != entry.indices.cend();
           ++jt) {
        int indexNum = jt->first;
        int sectorNum = jt->second;
        ofs << "    INDEX "
            << std::setfill('0') << std::setw(2) << indexNum
            << " ";
        
        CdMsf msf;
        msf.fromSectorNum(sectorNum);
        ofs << std::setfill('0') << std::setw(2) << msf.min
            << ":"
            << std::setfill('0') << std::setw(2) << msf.sec
            << ":"
            << std::setfill('0') << std::setw(2) << msf.frame
            << std::endl;
      }
    }
  }
  
  void CdBuilder::setXaEorFlagMode(
      XaSubheaderFlagModes::XaSubheaderFlagMode mode) {
    xaEorFlagMode_ = mode;
  }
  
  void CdBuilder::setXaEofFlagMode(
      XaSubheaderFlagModes::XaSubheaderFlagMode mode) {
    xaEofFlagMode_ = mode;
  }
  
  void CdBuilder::computeSectorEcc(const BlackT::TByte* src,
                         BlackT::TByte* dst,
                         BlackT::TGaloisField256Cd& field
//                         BlackT::TGaloisField256Polynomial& generator
                         ) const {
    // p-vectors
    // k = selector for even/odd bytes
    for (int k = 1; k >= 0; k--) {
      for (int i = 0; i < 43; i++) {
        BlackT::TGaloisField256Polynomial message(24);
        for (int j = 0; j < 24; j++) {
          int wordNum = (i) + (j * 43);
          int offset = (wordNum * 2) + k;
          message[j] = src[offset];
        }
      
//        std::cerr << "message: " << std::dec << message.size() << ": ";
//        for (int i = 0; i < message.size(); i++) {
//          std::cerr << std::hex << (int)message[i] << " ";
//        }
//        std::cerr << std::endl;
    
        BlackT::TGaloisField256Polynomial result = field.calcReedSolomonCode(
          message, 26);
        
//        std::cerr << "code: " << std::dec << result.size() << ": ";
//        for (int i = 0; i < result.size(); i++) {
//          std::cerr << std::hex << (int)result[i] << " ";
//        }
//        std::cerr << std::endl;
        
        // output the x^1 coefficient to (2076 + (i * 2)+ k)
        // output the x^0 coefficient to (2162 + (i * 2) + k)
        
        dst[0 + (i * 2) + k] = (TByte)result[24];
        dst[86 + (i * 2) + k] = (TByte)result[25];
      }
    }
    
    // q-vectors
    // k = selector for even/odd bytes
    for (int k = 1; k >= 0; k--) {
      for (int i = 0; i < 26; i++) {
        
        TGaloisField256Polynomial message(43);
        for (int j = 0; j < 43; j++) {
//          ifs.seek(12 + ((((i * 43) + (j * 44)) % 1118) * 2));
          int offset = ((((i * 43) + (j * 44)) % 1118) * 2) + k;
          message[j] = src[offset];
        }
        
  //      cout << "message: " << dec << message.size() << ": ";
  //      for (int i = 0; i < message.size(); i++) {
  //        cout << hex << (int)message[i] << " ";
  //      }
  //      cout << endl;
    
        TGaloisField256Polynomial result = field.calcReedSolomonCode(
          message, 45);
        
//        cout << "code: " << dec << result.size() << ": ";
//        for (int i = 0; i < result.size(); i++) {
//          cout << hex << (int)result[i] << " ";
//        }
//        cout << endl;
        
        // output the x^1 coefficient to (2248 + (i * 2))
        // output the x^0 coefficient to (2300 + (i * 2))
        
        dst[172 + (i * 2) + k] = (TByte)result[43];
        dst[224 + (i * 2) + k] = (TByte)result[44];
        
      }
    }
  }
  
  unsigned int CdBuilder
      ::computeEdc(const BlackT::TByte* src, int len) const {
    TEdcCalc edcCalc;
    return edcCalc.calcEdc(src, len);
  }
  
  int CdBuilder::byteToSectorPos(int bytePos) {
    return (bytePos / CdConsts::physicalSectorSize);
  }
  
  int CdBuilder::sectorToBytePos(int sectorPos) {
    return (sectorPos * CdConsts::physicalSectorSize);
  }
  
  int CdBuilder::byteToSectorLocalPos(int bytePos) {
    return (bytePos % CdConsts::physicalSectorSize);
  }
  
/*  int CdBuilder::sectorDataAreaStartPos(CdModeIds::CdModeId modeId) {
    switch (modeId) {
    case CdModeIds::mode1:
      return CdConsts::mode1DataAreaStartOffset;
      break;
    case CdModeIds::mode2form1:
    case CdModeIds::mode2form2:
      return CdConsts::mode2DataAreaStartOffset;
      break;
    default:
      return 0;
      break;
    }
  }
  
  int CdBuilder::sectorDataAreaEndPos(CdModeIds::CdModeId modeId) {
    switch (modeId) {
    case CdModeIds::mode1:
      return CdConsts::mode1DataAreaEndOffset;
      break;
    case CdModeIds::mode2form1:
      return CdConsts::mode2Form1DataAreaEndOffset;
      break;
    case CdModeIds::mode2form2:
      return CdConsts::mode2Form2DataAreaEndOffset;
      break;
    default:
      return CdConsts::physicalSectorSize;
      break;
    }
  } */
  
  bool CdBuilder::sectorIsActive() const {
    return (dataAreaEndPos_ != -1);
  }
  
  bool CdBuilder::trackIsActive() const {
    return (currentTrackNum_ != -1);
  }
    
  void CdBuilder::padToSectorBoundary() {
    // do nothing if already at sector boundary
    if (!sectorIsActive()) return;
    
    finishSector();
  }
  
  void CdBuilder::addNullSectors(int numSectors) {
    if (sectorIsActive()) padToSectorBoundary();
    
//    CdModeIds::CdModeId old_currentSectorMode = currentSectorMode_;
    
    for (int i = 0; i < numSectors; i++) {
//      prepNextSector(currentSectorMode_);
//      finishSector();
      prepNextSector(CdModeIds::null);
      finishSector();
    }
    
//    currentSectorMode_ = old_currentSectorMode;
  }
  
  void CdBuilder::addEmptySectors(int numSectors) {
    if (sectorIsActive()) padToSectorBoundary();
    
    // save XA flags
    bool old_xaVideoFlag = xaVideoFlag_;
    bool old_xaAudioFlag = xaAudioFlag_;
    bool old_xaDataFlag = xaDataFlag_;
    bool old_xaTriggerFlag = xaTriggerFlag_;
    bool old_xaRealTimeFlag = xaRealTimeFlag_;
    
    // zero the XA flags
    xaVideoFlag_ = false;
    xaAudioFlag_ = false;
    xaDataFlag_ = false;
    xaTriggerFlag_ = false;
    xaRealTimeFlag_ = false;
    
    for (int i = 0; i < numSectors; i++) {
//      prepNextSector(currentSectorMode_);
//      finishSector();
      prepNextSector(currentSectorMode_);
      finishSector();
    }
    
    xaVideoFlag_ = old_xaVideoFlag;
    xaAudioFlag_ = old_xaAudioFlag;
    xaDataFlag_ = old_xaDataFlag;
    xaTriggerFlag_ = old_xaTriggerFlag;
    xaRealTimeFlag_ = old_xaRealTimeFlag;
  }
  
  void CdBuilder::addPadSectors(int numSectors) {
    if (sectorIsActive()) padToSectorBoundary();
    
    for (int i = 0; i < numSectors; i++) {
      prepNextSector(currentSectorMode_);
      finishSector();
    }
  }
  
  void CdBuilder::addIsoFilesystem(Object& env, const Object& isoOrig) {
    
    //======================================
    // set up
    //======================================
    
    // ensure initial sector alignment
    padToSectorBoundary();
    
    // copy iso object
//    isoFilesystems_.push_back(isoOrig);
//    Object& iso = isoFilesystems_.back();
    isosWithAssociates_.push_back(IsoWithAssociates());
    IsoWithAssociates& isoWithAssociates = isosWithAssociates_.back();
    
    isoWithAssociates.iso = isoOrig;
    Object& iso = isoWithAssociates.iso;
    
    isoWithAssociates.typeLPathTable = IsoPathTable();
    isoWithAssociates.typeLPathTableCopy = IsoPathTable();
    isoWithAssociates.typeMPathTable = IsoPathTable();
    isoWithAssociates.typeMPathTableCopy = IsoPathTable();
    
    // save iso start position
    int startSector = currentSectorNum_;
    iso.setMember("build_startSector", currentSectorNum_);
    
    // get CD format (cdrom/cdxa)
    std::string cdFormatString = iso.getMemberString("format");
    CdFormatIds::CdFormatId cdFormatId
      = CdConsts::formatStringToId(cdFormatString);
    
    //======================================
    // generate self/parent reference
    // dummy file entries in each directory
    // listing
    //======================================
    
    IsoFilesystem::addSelfAndParentPointers(&env, &iso);
    
    //======================================
    // add system area.
    // system area inherits current sector
    // mode
    //======================================
    
    const std::string& systemArea = iso.getMember("systemArea").stringValue();
    
    if (iso.getMemberInt("systemAreaIsRawSector") != 0) {
      if (config.debugOutput()) {
        std::cout << "Sector " << currentSectorNum() << ": "
          << "Writing system area (from raw-sector data)"
          << std::endl;
      }
      
      // trust that the user will give us reasonable input...
      TBufStream ifs;
      ifs.write((char*)systemArea.c_str(), systemArea.size());
      ifs.seek(0);
      writeRawSectorData(ifs);
    }
    else {
      if (config.debugOutput()) {
        std::cout << "Sector " << currentSectorNum() << ": "
          << "Writing system area"
          << std::endl;
      }
      
      // determine how much of the 16-sector space the provided data will fill
      int systemAreaDataSize = systemArea.size();
      int sectorDataSize = CdConsts::sectorDataAreaSize(currentSectorMode_);
      int systemAreaTotalSize
        = sectorDataSize * CdConsts::numIsoSystemAreaSectors;
      int systemAreaLeftoverSectors
        = (systemAreaTotalSize - systemAreaDataSize) / sectorDataSize;
      
      // write the provided data
      writeData((BlackT::TByte*)systemArea.c_str(), systemArea.size());
      
      // pad as needed to reach 16 sectors
      addPadSectors(systemAreaLeftoverSectors);
    }
    
    //======================================
    // evaluate build list
    //======================================
    
    const Object& buildListObj = iso.getMember("buildList");
    const ObjectArray& buildList = buildListObj.arrayValue();
    for (ObjectArray::const_iterator it = buildList.begin();
         it != buildList.end();
         ++it) {
      const Object& buildCmdObj = *it;
      std::string buildCmdName = buildCmdObj.type();
      
      if (buildCmdName.compare("addPrimaryVolumeDescriptor") == 0) {
//        isoPrimaryVolumeDescriptors_.push_back(
//          buildCmdObj.getMember("primaryVolumeDescriptor"));
        isoWithAssociates.primaryVolumeDescriptors.push_back(
          buildCmdObj.getMember("primaryVolumeDescriptor"));
        Object& primaryVolumeDescriptor
          = isoWithAssociates.primaryVolumeDescriptors.back();
        
        padToSectorBoundary();
        
        primaryVolumeDescriptor
          .setMember("build_absSectorNum", currentSectorNum_);
        
        if (config.debugOutput()) {
          std::cout << "Sector " << currentSectorNum() << ": "
            << "Writing primary volume descriptor"
            << std::endl;
        }
        
        TBufStream ofs;
        IsoPrimaryVolumeDescriptor::write(&env, &primaryVolumeDescriptor,
          ofs, cdFormatId);
        ofs.seek(0);
        
//        writeData((TByte*)ofs.data().data(), ofs.size());
        writeDataWithXaFlags(ofs,
                             XaSubheaderFlagModes::everySector,
                             XaSubheaderFlagModes::none);
        
        padToSectorBoundary();
      }
      else if (buildCmdName.compare("addDescriptorSetTerminator") == 0) {
        // we don't have to remember these because they don't have any
        // fields that would need to be updated later in the build
        const Object& descriptorSetTerminator = 
          buildCmdObj.getMember("descriptorSetTerminator");
        
        if (config.debugOutput()) {
          std::cout << "Sector " << currentSectorNum() << ": "
            << "Writing volume descriptor set terminator"
            << std::endl;
        }
        
        TBufStream ofs;
        IsoDescriptorSetTerminator::write(&env, &descriptorSetTerminator, ofs);
        ofs.seek(0);
        
//        writeData((TByte*)ofs.data().data(), ofs.size());
        writeDataWithXaFlags(ofs,
                             XaSubheaderFlagModes::everySector,
//                             XaSubheaderFlagModes::lastSector);
                             XaSubheaderFlagModes::everySector);
      }
      else if (buildCmdName.compare("addDirectoryDescriptor") == 0) {
        std::string path = 
          buildCmdObj.getMemberString("directoryName");
        
//        TBufStream dst;
        writeDirectoryDescriptor(env, iso, path, cdFormatId);
      }
      else if (buildCmdName.compare("addListedFile") == 0) {
        std::string filename = 
          buildCmdObj.getMemberString("filename");
        
        Object& fileListingObj = *IsoFilesystem
          ::getDirectoryListingObjectFromPath(&env, &iso, filename);
        
        if (fileListingObj.getMemberInt("isDirectory") != 0) {
          throw TGenericException(T_SRCANDLINE,
                                  "addIsoFilesystem()",
                                  std::string("Tried to add listed directory ")
                                    + "\""
                                    + filename
                                    + "\""
                                    + " as a file");
        }
        
        if (fileListingObj.type().compare("DiscPointerListing") == 0) {
          throw TGenericException(T_SRCANDLINE,
                                  "addIsoFilesystem()",
                                  std::string("Tried to add disc pointer ")
                                    + "\""
                                    + filename
                                    + "\""
                                    + " as a file");
        }
        
        std::string sourceFilename
          = fileListingObj.getMemberString("sourceFilename");
        
/*        if (!TFileManip::fileExists(sourceFilename)) {
          throw TGenericException(T_SRCANDLINE,
                                  "addIsoFilesystem()",
                                  std::string("Tried to add nonexistent ")
                                    + "source file "
                                    + "\""
                                    + sourceFilename
                                    + "\""
                                    + " to disc as "
                                    + "\""
                                    + filename
                                    + "\"");
        } */
        
        padToSectorBoundary();
        
        CdModeIds::CdModeId oldSectorMode = currentSectorMode_;
        bool old_xaVideoFlag = xaVideoFlag_;
        bool old_xaAudioFlag = xaAudioFlag_;
        bool old_xaDataFlag = xaDataFlag_;
        bool old_xaTriggerFlag = xaTriggerFlag_;
        bool old_xaRealTimeFlag = xaRealTimeFlag_;
        
        // for XA discs, check file mode/form settings and set sector mode
        // appropriately for duration of file
        if (cdFormatId == CdFormatIds::cdxa) {
          // mode2
          if (fileListingObj.getMemberInt("xa_flags_isMode2Form2") != 0) {
            // form2
            currentSectorMode_ = CdModeIds::mode2form2;
          }
          else if (fileListingObj.getMemberInt("xa_flags_isMode2") != 0) {
            // form1
            currentSectorMode_ = CdModeIds::mode2form1;
          }
          // TODO: ?
          // i'm guessing these are actually disc pointers (outside of
          // filesystem) but don't have any way to check
          else if (fileListingObj.getMemberInt("xa_flags_isCdda") != 0) {
            // audio
            currentSectorMode_ = CdModeIds::audio;
          }
          else {
            // mode1
//            currentSectorMode_ = CdModeIds::mode1;
            // inherit current track mode
          }
          
          xaVideoFlag_
            = (fileListingObj.getMemberInt("xa_submode_video") != 0);
          xaAudioFlag_
            = (fileListingObj.getMemberInt("xa_submode_audio") != 0);
          xaDataFlag_
            = (fileListingObj.getMemberInt("xa_submode_data") != 0);
          xaTriggerFlag_
            = (fileListingObj.getMemberInt("xa_submode_trigger") != 0);
          xaRealTimeFlag_
            = (fileListingObj.getMemberInt("xa_submode_realTime") != 0);
        }
        
        // write disc location to source object
        // (note: absolute disc offset. must subtract ISO start sector
        // when computing offsets)
        fileListingObj.setMember("build_absSectorNum", currentSectorNum_);
//        int startPos = currentSectorNum_;
        fileListingObj.setMember("dataOffset",
          getLogicalBlockOffset(&env, &iso, currentSectorNum_));
        
        if (fileListingObj.getMemberInt("usesRawSectors") != 0) {
          // if a raw-sector file, do raw copy
          if (config.debugOutput()) {
            std::cout << "Sector " << currentSectorNum() << ": "
              << "Performing raw-sector write of local file "
              << "\""
              << sourceFilename
              << "\""
              << " to disc as "
              << "\""
              << filename
              << "\""
              << std::endl;
          }
          
          if (!TFileManip::fileExists(sourceFilename)) {
            throw TGenericException(T_SRCANDLINE,
                                    "CdBuilder::addIsoFilesystem()",
                                    std::string(
                                      "Tried to write data from nonexistent ")
                                      + "raw-sector file "
                                      + sourceFilename);
          }
          
          // set mode to match target file
//          std::string modestring = fileListingObj.getMemberString(
//            "rawSectorModestring");
//          currentSectorMode_ = CdConsts::modeStringToId(modestring);
          // unfortunately, this is not good enough.
          // raw-sector files may need to contain multiple sector types
          // (particularly, i've encountered files with interleaved
          // form1/form2 sectors).
          // so instead, this must be set on the fly from each sector's
          // header data.
          
          TIfstream ifs(sourceFilename.c_str(), std::ios_base::binary);
          writeRawSectorData(ifs);
        }
        else {
          if (config.debugOutput()) {
            std::cout << "Sector " << currentSectorNum() << ": "
              << "Writing local file "
              << "\""
              << sourceFilename
              << "\""
              << " to disc as "
              << "\""
              << filename
              << "\""
              << std::endl;
          }
        
          // "The EOR bit is set in all Volume Descriptor sectors, the last
          // sector (ie. the Volume Descriptor Terminator) additionally has
          // the EOF bit set. Moreover, EOR and EOF are set in the last sector
          // of each Path Table, and last sector of each Directory, and last
          // sector of each File."
          
  //          writeDataFromFile(sourceFilename);
          writeDataFromFileWithXaFlags(sourceFilename,
                                       XaSubheaderFlagModes::lastSector,
                                       XaSubheaderFlagModes::lastSector);
        }
        
        padToSectorBoundary();
        
        currentSectorMode_ = oldSectorMode;
        
        xaVideoFlag_ = old_xaVideoFlag;
        xaAudioFlag_ = old_xaAudioFlag;
        xaDataFlag_ = old_xaDataFlag;
        xaTriggerFlag_ = old_xaTriggerFlag;
        xaRealTimeFlag_ = old_xaRealTimeFlag;
        
//        int endPos = currentSectorNum_;
//        int sectorSize = endPos - startPos;
      }
      else if (buildCmdName.compare("addNullSectors") == 0) {
        int numSectors = buildCmdObj.getMemberInt("numSectors");
        
        if (config.debugOutput()) {
          std::cout << "Sector " << currentSectorNum() << ": "
            << "Writing "
            << numSectors
            << " null sector(s) to ISO"
            << std::endl;
        }
        
        addNullSectors(numSectors);
      }
      else if (buildCmdName.compare("addEmptySectors") == 0) {
        int numSectors = buildCmdObj.getMemberInt("numSectors");
        
        if (config.debugOutput()) {
          std::cout << "Sector " << currentSectorNum() << ": "
            << "Writing "
            << numSectors
            << " empty sector(s) to ISO"
            << std::endl;
        }
        
        addEmptySectors(numSectors);
      }
      else if (buildCmdName.compare("addTypeLPathTable") == 0) {
        isoWithAssociates.typeLPathTable = generatePathTable(env, iso);
        writePathTable(env, isoWithAssociates.typeLPathTable,
                       EndiannessTypes::little);
      }
      else if (buildCmdName.compare("addTypeLPathTableCopy") == 0) {
        isoWithAssociates.typeLPathTableCopy = generatePathTable(env, iso);
        writePathTable(env, isoWithAssociates.typeLPathTableCopy,
                       EndiannessTypes::little);
      }
      else if (buildCmdName.compare("addTypeMPathTable") == 0) {
        isoWithAssociates.typeMPathTable = generatePathTable(env, iso);
        writePathTable(env, isoWithAssociates.typeMPathTable,
                       EndiannessTypes::big);
      }
      else if (buildCmdName.compare("addTypeMPathTableCopy") == 0) {
        isoWithAssociates.typeMPathTableCopy = generatePathTable(env, iso);
        writePathTable(env, isoWithAssociates.typeMPathTableCopy,
                       EndiannessTypes::big);
      }
//      else if (buildCmdName.compare("addDirectoryContentFully") == 0) {
//        
//      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "addIsoFilesystem()",
                                std::string("Unknown build command: ")
                                  + buildCmdName);
      }
      
    }
    
    //======================================
    // record total iso size
    //======================================
    
    padToSectorBoundary();
    
    int totalSectorSize = currentSectorNum_ - startSector;
    iso.setMember("build_totalSectorSize", totalSectorSize);
    
  }
  
  Object CdBuilder::generatePathTable(Object& env, Object& iso) {
    IsoPathTable table;
    IsoPathTable::fromFileListing(&env, &table,
      &(iso.getMember("directoryListing")));
    return table;
  }
    
  void CdBuilder::writePathTable(Object& env, Object& pathTable,
                       BlackT::EndiannessTypes::EndiannessType endianness) {
    padToSectorBoundary();
    
    if (config.debugOutput()) {
      std::cout << "Sector " << currentSectorNum() << ": "
        << "Writing path table (endianness: "
        << ((endianness == EndiannessTypes::little)
            ? std::string("little") : std::string("big"))
        << ")"
        << std::endl;
    }
    
    TBufStream dst;
    IsoPathTable::write(&env, &pathTable, dst, endianness);
    dst.seek(0);
    
    pathTable.setMember("build_absSectorNum", currentSectorNum_);
    
//    writeData((TByte*)dst.data().data(), dst.size());
    writeDataWithXaFlags(dst,
                         XaSubheaderFlagModes::lastSector,
                         XaSubheaderFlagModes::lastSector);
    
    pathTable.setMember("build_size", dst.size());
    
    padToSectorBoundary();
  
//    static void write(
//        const Object* env, const Object* self, BlackT::TStream& dst,
//        BlackT::EndiannessTypes::EndiannessType endianness);
  }
    
  void CdBuilder::updatePathTable(Object& env, Object& iso, Object& pathTable,
                       BlackT::EndiannessTypes::EndiannessType endianness) {
    if (!pathTable.hasMember("build_absSectorNum")) return;
    
    int tableSectorOffset = pathTable.getMemberInt("build_absSectorNum");
    
    ObjectArray& pathArray = pathTable.getMember("pathArray").arrayValue();
    for (ObjectArray::iterator it = pathArray.begin();
         it != pathArray.end();
         ++it) {
      Object& pathTableEntry = *it;
      std::string fullPath = pathTableEntry.getMemberString("fullPath");
      
      Object& fileListingObj = *IsoFilesystem
        ::getDirectoryListingObjectFromPath(&env, &iso, fullPath);
      if (!fileListingObj.hasMember("build_absSectorNum")) {
        if (config.warningsOn()) {
          std::cerr << "WARNING: "
            << "Cannot complete path table entry for directory "
            << "\""
            << fullPath
            << "\""
            << " because it has no directory descriptor placed on disc"
            << std::endl;
        }
      }
      else {
        int directoryRecordSectorNum = fileListingObj.getMemberInt(
          "build_absSectorNum");
        
        pathTableEntry.getMember("logicalBlockNumber").setIntValue(
          getLogicalBlockOffset(&env, &iso, directoryRecordSectorNum));
      }
    }
    
    TBufStream dst;
    IsoPathTable::write(&env, &pathTable, dst, endianness);
    dst.seek(0);
    
    beginOverwriteAtSector(tableSectorOffset);
      if (config.debugOutput()) {
        std::cout << "Sector " << currentSectorNum() << ": "
          << "Overwriting path table with finalized values (endianness: "
          << ((endianness == EndiannessTypes::little)
              ? std::string("little") : std::string("big"))
          << ")"
          << std::endl;
      }
      
//      writeData((TByte*)dst.data().data(), dst.size());
      writeDataWithXaFlags(dst,
                           XaSubheaderFlagModes::lastSector,
                           XaSubheaderFlagModes::lastSector);
    endOverwrite();
  }
  
  void CdBuilder::writeDirectoryDescriptor(Object& env, Object& iso,
                              std::string path,
                              CdFormatIds::CdFormatId cdFormatId) {
//    std::string path = 
//      buildCmdObj.getMemberString("directoryName");
    
    Object* targetDirectory
      = IsoFilesystem::getDirectoryListingObjectFromPath(&env, &iso, path);
    
    // flag as placed on disc
    targetDirectory->setMember("build_written", 1);
    
    writeDirectoryDescriptor(env, iso, targetDirectory, path, cdFormatId);
  }
  
  void CdBuilder::writeDirectoryDescriptor(Object& env, Object& iso,
                              Object* targetDirectory,
                              std::string path,
                              CdFormatIds::CdFormatId cdFormatId) {
    TBufStream dst;
    
    // align to boundary
    padToSectorBoundary();
    
    if (config.debugOutput()) {
      std::cout << "Sector " << currentSectorNum() << ": "
        << "Writing directory descriptor for folder "
        << "\""
        << path
        << "\""
        << std::endl;
    }
      
    if (targetDirectory->getMemberInt("isDirectory") == 0) {
        throw TGenericException(T_SRCANDLINE,
                                "writeDirectoryDescriptor()",
                                std::string("Tried to write directory ")
                                  + "descriptor for non-directory: \""
                                  + path
                                  + "\"");
    }
    
    // Sort files in directory by ISO rules
    std::vector<Object*> sortVec;
    Object& dirMap = *FileListing::getDirectoryMap(&env, targetDirectory);
    for (ObjectMemberMap::iterator it = dirMap.members().begin();
         it != dirMap.members().end();
         ++it) {
      sortVec.push_back(&(it->second));
    }
    
    // get self record
//    Object selfListingObj = *targetDirectory;
//    selfListingObj.getMember("name").setStringValue("\x00");
    std::string selfListingString;
    selfListingString += CdConsts::directoryRecordSelfReferenceChar;
    Object& selfListingObj
      = *FileListing::getFile(&env, targetDirectory, selfListingString);
    
    // add parent record
//    Object parentListingObj
//      = *IsoFilesystem::getDirectoryListingObjectFromPath(&env, &iso,
//          targetDirectory->getMemberString("parentPath"));
//    parentListingObj.getMember("name").setStringValue("\x01");

//    std::string parentListingString;
//    parentListingString += CdConsts::directoryRecordParentReferenceChar;
//    Object& parentListingObj
//      = *FileListing::getFile(&env, targetDirectory, parentListingString);
    
    std::sort(sortVec.begin(), sortVec.end(), FileListing::sortByIso9660NameP);
    
    // record start position
    int recordStartLogicalBlockOffset
      = getLogicalBlockOffset(&env, &iso, currentSectorNum_);
    targetDirectory->setMember("build_absSectorNum", currentSectorNum_);
    targetDirectory->setMember("dataOffset", recordStartLogicalBlockOffset);
    // also record to self-reference
    selfListingObj.setMember("build_absSectorNum", currentSectorNum_);
    selfListingObj.setMember("dataOffset", recordStartLogicalBlockOffset);
    
    
    int descriptorStartSector = currentSectorNum_;
//    int totalDescriptorSize = 0;
    
    // write directory record for each file
    for (std::vector<Object*>::iterator it = sortVec.begin();
         it != sortVec.end();
         ++it) {
      Object& fileObj = *(*it);
//      std::cerr << fileObj.getMemberString("name") << std::endl;
      
      if (config.debugOutput()) {
        std::string name = fileObj.getMemberString("name");
        
        if (name.size() == 0) {
          name = "<root>";
        }
        else if ((name.size() == 1)
            && (name[0] == CdConsts::directoryRecordSelfReferenceChar)) {
          name = "<self>";
        }
        else if ((name.size() == 1)
            && (name[1] == CdConsts::directoryRecordParentReferenceChar)) {
          name = "<parent>";
        }
        else {
          name = "\"" + name + "\"";
        }
        
        std::cout << "  Writing directory record for "
          << name
          << std::endl;
      }
      
      TBufStream dst;
      FileListing::writeDirectoryRecord(
        &env, &fileObj, dst, cdFormatId);
      dst.seek(0);
      
      if (sectorIsActive()) {
        // easily missed line in the standard:
        // "Each Directory Record shall end in the Logical Sector in which it
        // begins."
        // if the next record won't fit in the sector, we have to pad to the
        // next one.
        int remainingSpace = remainingBytesInSector();
        if (dst.size() > remainingSpace) padToSectorBoundary();
      }

//      writeData((TByte*)dst.data().data(), dst.size());
      writeDataWithXaFlags(dst,
                           XaSubheaderFlagModes::lastSector,
                           XaSubheaderFlagModes::lastSector);
//      totalDescriptorSize += dst.size();
    }
    
    padToSectorBoundary();
    
    // size of directory includes end-of-sector padding
    int descriptorEndSector = currentSectorNum_;
    int descriptorSectorSize = descriptorEndSector - descriptorStartSector;
    int descriptorByteSize = descriptorSectorSize * CdConsts::logicalSectorSize;
    
//    targetDirectory->setMember("build_sectorSize", descriptorSectorSize);
    targetDirectory->setMember("dataSize", descriptorByteSize);
    selfListingObj.setMember("dataSize", descriptorByteSize);
  }
  
  int CdBuilder::getLogicalBlockOffset(
      Object* env, Object* iso, int sectorOffset) {
    return ((sectorOffset - iso->getMemberInt("build_startSector"))
      * (CdConsts::logicalSectorSize / iso->getMemberInt("logicalBlockSize")));
  }
  
  int CdBuilder::getLogicalBlockSize(
      Object* env, Object* iso, int sectorSize) {
    return (sectorSize
      * (CdConsts::logicalSectorSize / iso->getMemberInt("logicalBlockSize")));
  }
  
//  int CdBuilder::currentSectorNum() const {
//    return writePos_ / CdConsts::physicalSectorSize;
//  }
  
  void CdBuilder::prepNextSector(CdModeIds::CdModeId modeId) {
    // error if not at sector boundary
    if (sectorIsActive()) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::prepNextSector(CdModeIds::CdModeId)",
                              "Tried to prep sector while sector still active");
    }
    
    // error if at end of disc
    if (currentSectorNum_ == maxSectorCount_) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::prepNextSector(CdModeIds::CdModeId)",
                              std::string("Tried to add sector to full disc ")
                                + "(sector "
                                + TStringConversion::toString(maxSectorCount_)
                                + " reached)");
    }
    
    // byte at which new sector begins
//    int sectorBasePos = writePos_;
    int sectorBasePos = sectorToBytePos(currentSectorNum_);
    writePos_ = sectorBasePos;
    
    int dataAreaStart = CdConsts::sectorDataAreaStartPos(modeId);
    int dataAreaEnd = CdConsts::sectorDataAreaEndPos(modeId);
    
    // if sector already formatted, do not do any of the prep write operations.
    // this is used when overwriting data in old sectors.
    if (sectorModeMap_[currentSectorNum_] == 0xFF) {
      // mark sector mode
//      currentSectorMode_ = modeId;
      sectorModeMap_[currentSectorNum_] = (TByte)modeId;
      
      // zero-fill new sector
      cdbuf_.write((char*)blankSectorArray_, sizeof(blankSectorArray_));
      // seek to start of sector
      cdbuf_.seekp(discStreamBasePos_ + sectorBasePos);
    
      // for audio sectors, no headers: we're done
      if ((modeId == CdModeIds::audio)
          || (modeId == CdModeIds::mode1raw)
          || (modeId == CdModeIds::null)) {
        dataAreaEndPos_ = sectorBasePos + CdConsts::physicalSectorSize;
        return;
      }
    
      // sync field
      cdbuf_.write(
        (char*)sectorSyncField_, sizeof(sectorSyncField_));
      
      // sector position (including lead-in pregap)
      int discAbsoluteSectorNum = currentSectorNum_ + initialPregapSectors_;
      CdMsf msf;
      msf.fromSectorNumToBcd(discAbsoluteSectorNum);
      cdbuf_.put((char)((unsigned char)msf.min));
      cdbuf_.put((char)((unsigned char)msf.sec));
      cdbuf_.put((char)((unsigned char)msf.frame));
      
      // mode
      switch (modeId) {
      case CdModeIds::mode0:
        cdbuf_.put(0x00);
        break;
      case CdModeIds::mode1:
        cdbuf_.put(0x01);
        break;
      case CdModeIds::mode2form1:
      case CdModeIds::mode2form2:
        cdbuf_.put(0x02);
        break;
      default:
        
        break;
      }
      
      // special XA stuff
      if ((modeId == CdModeIds::mode2form1)
          || (modeId == CdModeIds::mode2form2)) {
        
        // file number
        TByte fileNumber = xaFileNumber_;
        
        // channel number
        TByte channelNumber = xaChannelNumber_ & 0x1F;
        
        // submode
        TByte xaSubmodeFlags = 0x00;
        if (newMode2SectorsAreEor_)
          xaSubmodeFlags |= CdConsts::xaSubmodeEorMask;
        if (xaVideoFlag_)
          xaSubmodeFlags |= CdConsts::xaSubmodeVideoMask;
        if (xaAudioFlag_)
          xaSubmodeFlags |= CdConsts::xaSubmodeAudioMask;
        if (xaDataFlag_)
          xaSubmodeFlags |= CdConsts::xaSubmodeDataMask;
        if (xaTriggerFlag_)
          xaSubmodeFlags |= CdConsts::xaSubmodeTriggerMask;
        if (modeId == CdModeIds::mode2form2)
          xaSubmodeFlags |= CdConsts::xaSubmodeForm2Mask;
        if (xaRealTimeFlag_)
          xaSubmodeFlags |= CdConsts::xaSubmodeRealTimeMask;
        if (newMode2SectorsAreEof_)
          xaSubmodeFlags |= CdConsts::xaSubmodeEofMask;
        
        // TODO: codinginfo byte (XA-ADPCM)
        TByte codingInfo = 0;
        
        // write subheader
        cdbuf_.seekp(discStreamBasePos_ + sectorBasePos
                      + CdConsts::mode2SubheaderStartOffset);
        cdbuf_.put((char)fileNumber);
        cdbuf_.put((char)channelNumber);
        cdbuf_.put((char)xaSubmodeFlags);
        cdbuf_.put((char)codingInfo);
      }
      
/*      switch (modeId) {
      case CdModeIds::mode2form2:
        // mark as form2
        cdbuf_.seekp(discStreamBasePos_ + sectorBasePos
                      + CdConsts::mode2SubheaderStartOffset
                      + 2);
        cdbuf_.put(0x20);
        break;
      default:
        
        break;
      } */
    
      // TODO: fill in appropriate header data for XA modes
      // TODO: using user-configurable settings for e.g. XA subheader data
      // ...
    }
    
    // set final write/end positions
    
    writePos_ = sectorBasePos + dataAreaStart;
    dataAreaEndPos_ = sectorBasePos + dataAreaEnd;
    
    cdbuf_.seekp(discStreamBasePos_ + writePos_);
  }
  
  void CdBuilder::finishSector() {
    // error if no sector active
    if (!sectorIsActive()) {
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::finishSector(CdModeIds::CdModeId)",
                              "Tried to finish sector with no sector active");
    }
    
    // advance position to next sector
    ++currentSectorNum_;
    writePos_ = sectorToBytePos(currentSectorNum_);
    // mark as uninitialized
    dataAreaEndPos_ = -1;
    
    // move stream to new write position
    cdbuf_.seekp(discStreamBasePos_ + writePos_);
  }
  
  CdModeIds::CdModeId CdBuilder::getSectorMode(int sectorNum) const {
    // TODO: validity checks?
//    if (sectorNum >= 
    
    return static_cast<CdModeIds::CdModeId>(sectorModeMap_[sectorNum]);
  }
  
  int CdBuilder::remainingBytesInSector() const {
    if (!sectorIsActive()) return 0;
    
    return dataAreaEndPos_ - writePos_;
  }


}
