#ifndef DISCASTERCDCONSTS_H
#define DISCASTERCDCONSTS_H


#include "util/TByte.h"
#include <fstream>
#include <iostream>
#include <string>

namespace Discaster {

  
  namespace CdModeIds {
    enum CdModeId {
      null,       // dummy entry for zero-filled sectors
      audio,
      mode0,      // empty sectors
      mode1,      // cd-rom with ecc
      mode1raw,   // cd-rom with no ecc?
                  // is this even real??
      mode2form1, // cd-xa with ecc
      mode2form2  // cd-xa no ecc
    };
  }
  
  namespace CdFormatIds {
    enum CdFormatId {
      cdrom,
      cdxa
    };
  }
  
  class CdConsts {
  public:
  
    //==========================================
    // functions
    //==========================================

    /**
     * Helper function to convert a CD modestring to an enum value.
     */
    static CdModeIds::CdModeId modeStringToId(std::string modestring);

    /**
     * Helper function to convert raw CD mode data to an enum value.
     * @param modeByte The mode byte (byte 0xF of the raw sector).
     * @param submodeByte The submode byte (byte 0x12/0x16 of the raw sector).
     *                     Only relevant for CD-XA.
     */
    static CdModeIds::CdModeId modeDataToId(
      unsigned char modeByte, unsigned char submodeByte);

    /**
     * Helper function to convert a CD mode enum value to a string.
     */
    static std::string idToModeString(CdModeIds::CdModeId modeId);

    /**
     * Helper function to convert a CD mode enum value to a string as used
     * in a CUE file.
     */
    static std::string idToCueStyleModeString(CdModeIds::CdModeId modeId);

    /**
     * Helper function to convert a CD formatstring to an enum value.
     */
    static CdFormatIds::CdFormatId formatStringToId(std::string str);

    /**
     * Helper function to convert a CD format enum value to a string.
     */
    static std::string idToFormatString(CdFormatIds::CdFormatId modeId);
    
    static int sectorDataAreaStartPos(CdModeIds::CdModeId modeId);
    static int sectorDataAreaEndPos(CdModeIds::CdModeId modeId);
    static int sectorDataAreaSize(CdModeIds::CdModeId modeId);
  
    //==========================================
    // consts
    //==========================================
  
    const static int physicalSectorSize = 0x930;
    const static int logicalSectorSize = 0x800;
    const static int cdCapacityInSectors = 0x514C8;
    const static int cdCapacityInBytes
      = cdCapacityInSectors * physicalSectorSize;
      
    // mode-dependent?
//    const static int cdRomSystemAreaSizeInBytes = 32768;
    
    const static int numInitialPregapSectors = 150;
    
    const static int numIsoSystemAreaSectors = 16;
    
    const static int sectorHeaderStartOffset = 0xC;
    const static int sectorHeaderModeOffset = 0xF;
    const static int sectorHeaderEndOffset = 0x10;
    
    const static int mode1DataAreaStartOffset = 0x10;
    const static int mode1DataAreaEndOffset = 0x810;
    const static int mode2DataAreaStartOffset = 0x18;
    const static int mode2Form1DataAreaEndOffset = 0x818;
    const static int mode2Form2DataAreaEndOffset = 0x92C;
    
    const static int mode1EccStartOffset = 0x81C;
    const static int mode1EccEndOffset = 0x930;
    const static int mode2Form1EccStartOffset = 0x81C;
    const static int mode2Form1EccEndOffset = 0x930;
    
    const static int mode1EccCheckedDataStartOffset = 0xC;
    const static int mode1EccCheckedDataEndOffset = 0x81C;
    const static int mode2Form1EccCheckedDataStartOffset = 0xC;
    const static int mode2Form1EccCheckedDataEndOffset = 0x81C;
    
    const static int mode1EdcStartOffset = 0x810;
    const static int mode1EdcEndOffset = 0x814;
    const static int mode2Form1EdcStartOffset = 0x818;
    const static int mode2Form1EdcEndOffset = 0x81C;
    const static int mode2Form2EdcStartOffset = 0x92C;
    const static int mode2Form2EdcEndOffset = 0x930;
    
    const static int mode1EdcCheckedDataStartOffset = 0x0;
    const static int mode1EdcCheckedDataEndOffset = 0x810;
    const static int mode2Form1EdcCheckedDataStartOffset = 0x10;
    const static int mode2Form1EdcCheckedDataEndOffset = 0x818;
    const static int mode2Form2EdcCheckedDataStartOffset = 0x10;
    const static int mode2Form2EdcCheckedDataEndOffset = 0x92C;
    
    const static int mode2SubheaderStartOffset = 0x10;
    const static int mode2SubheaderSubmodeOffset = 0x12;
    const static int mode2SubheaderEndOffset = 0x14;
    const static int mode2SubheaderCopyStartOffset = 0x14;
    const static int mode2SubheaderSubmodeCopyOffset = 0x16;
    const static int mode2SubheaderCopyEndOffset = 0x18;
    
    const static int maxHeaderAndSubheaderSize = 0x18;
    
    const static int primaryVolumeDescriptorTypeId = 0x01;
    const static int primaryVolumeDescriptorRootDirectoryRecordOffset = 156;
    
    const static int descriptorSetTerminatorTypeId = 0xFF;
    
    const static char directoryRecordSelfReferenceChar = '\x00';
    const static char directoryRecordParentReferenceChar = '\x01';
    
    const static int xaSubmodeEorShift      = 0;
    const static int xaSubmodeVideoShift    = 1;
    const static int xaSubmodeAudioShift    = 2;
    const static int xaSubmodeDataShift     = 3;
    const static int xaSubmodeTriggerShift  = 4;
    const static int xaSubmodeForm2Shift    = 5;
    const static int xaSubmodeRealTimeShift = 6;
    const static int xaSubmodeEofShift      = 7;
    
    const static int xaSubmodeEorMask      = (0x01 << xaSubmodeEorShift);
    const static int xaSubmodeVideoMask    = (0x01 << xaSubmodeVideoShift);
    const static int xaSubmodeAudioMask    = (0x01 << xaSubmodeAudioShift);
    const static int xaSubmodeDataMask     = (0x01 << xaSubmodeDataShift);
    const static int xaSubmodeTriggerMask  = (0x01 << xaSubmodeTriggerShift);
    const static int xaSubmodeForm2Mask    = (0x01 << xaSubmodeForm2Shift);
    const static int xaSubmodeRealTimeMask = (0x01 << xaSubmodeRealTimeShift);
    const static int xaSubmodeEofMask      = (0x01 << xaSubmodeEofShift);
  };


}


#endif
