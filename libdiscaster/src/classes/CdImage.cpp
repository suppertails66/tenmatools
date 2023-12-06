#include "classes/CdImage.h"
#include "classes/IsoFilesystem.h"
#include "discaster/CdConsts.h"
#include "discaster/CdBuilder.h"
#include "discaster/Config.h"
#include "discaster/CdMsf.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TFileManip.h"
#include "util/TSoundFile.h"
#include "exception/TGenericException.h"
#include <iostream>
#include <cstring>
#include <cctype>
#include <fstream>

using namespace BlackT;

namespace Discaster {

  /**
   * Helper function to add a build command to a CdImage build command list.
   */
  static void addBuildCommand(Object* self, const Object& command) {
    if (self != NULL) {
     self->getMember("buildList").arrayValue().push_back(command);
   }
  }

  /**
   * Begins a new CD track.
   * If another track is already active, will also end that track.
   * Disc content need not be part of any track, but exercise caution
   * when doing so -- CD players probably won't like having random unlisted
   * data between tracks.
   *
   * arg0 trackNum Track number.
   *               Tracks are not required to be sequential (but should be to
   *               produce a spec-conformant disc).
   * arg1 modeName String consisting of a track mode specifier.
   *               The following modes are valid:
   *                 - "AUDIO"
   *                 - "MODE1" (i.e. MODE1/2048)
   *                 - _NOT_ "MODE1RAW" (i.e. MODE1/2352).
   *                   i'm not convinced this is a real format.
   *                 - "MODE2FORM1" (i.e. MODE2/2336)
   *                 - "MODE2FORM2" (i.e. MODE2/2352)
   */
  static ParseNode* addTrackStart(Object* env, Object* self, ObjectArray args) {
    int trackNum = args.at(0).intValue();
    std::string modeName = args.at(1).stringValue();
    
    Object obj = BlobObject("addTrackStart");
    obj.setMember("trackNum", trackNum);
//    obj.setMember("modeName", modeName);
    obj.setMember("modeId", (int)(CdConsts::modeStringToId(modeName)));
    addBuildCommand(self, obj);
    
    return NULL;
  }

  /**
   * Ends the currently active CD track.
   */
  static ParseNode* addTrackEnd(Object* env, Object* self, ObjectArray args) {
//    int trackNum = args.at(0).intValue();
    
    Object obj = BlobObject("addTrackEnd");
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds an index to the active CD track.
   *
   * arg0 indexNum Index number.
   *               * Index 0 indicates the beginning of the track pregap (if any).
   *               * Index 1 indicates the end of the track pregap (if any) and the
   *                 start of the actual track data. It is normally mandatory.
   *               Other indices have application-defined meaning.
   */
  static ParseNode* addTrackIndex(Object* env, Object* self, ObjectArray args) {
    int indexNum = args.at(0).intValue();
    
    Object obj = BlobObject("addTrackIndex");
    obj.setMember("indexNum", indexNum);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds zero-filled data sectors to CD.
   * The generated sectors will be totally blank (no mode, no sync data, etc.)
   *
   * arg0 numSectors Count of sectors to generate.
   */
  static ParseNode* addNullSectors(Object* env, Object* self, ObjectArray args) {
    int numSectors = args.at(0).intValue();
    
    Object obj = BlobObject("addNullSectors");
    obj.setMember("numSectors", numSectors);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds empty data sectors to CD.
   * These will be generated according to the current disc mode (e.g. MODE1)
   * and will contain the relevant sector header data, but the data area will
   * be blank.
   *
   * arg0 numSectors Count of sectors to generate.
   */
  static ParseNode* addEmptySectors(Object* env, Object* self, ObjectArray args) {
    int numSectors = args.at(0).intValue();
    
    Object obj = BlobObject("addEmptySectors");
    obj.setMember("numSectors", numSectors);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds a label to the CD.
   * When this build step is process, the current write position on the CD
   * (with sector precision) will be associated with the provided name,
   * and can be referenced via certain other functions.
   *
   * arg0 labelName String identifier for the label.
   */
  static ParseNode* addLabel(Object* env, Object* self, ObjectArray args) {
    DiscasterString labelName = args.at(0).stringValue();
    
    Object obj = BlobObject("addLabel");
    obj.setMember("labelName", labelName);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds raw binary data from a file to the CD in the current mode.
   * NOTE: Sector alignment is not automatically enforced. If the data
   * does not take up an exact number of sectors, the next raw data
   * written after it will resume from the exact end of the previous
   * data rather than the start of the next sector.
   *
   * arg0 inputFileName Name of the input file.
   */
  static ParseNode* addRawData(Object* env, Object* self, ObjectArray args) {
    DiscasterString inputFileName = args.at(0).stringValue();
    
    Object obj = BlobObject("addRawData");
    obj.setMember("inputFileName", inputFileName);
    obj.setMember("skippedInitialSectors", 0);
    obj.setMember("totalSize", -1);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds raw binary data from a file to the CD in the current mode,
   * skipping over a specified number of sectors at the start of the data.
   * See addRawData().
   *
   * arg0 inputFileName Name of the input file.
   */
  static ParseNode* addRawDataWithSkippedInitialSectors(
      Object* env, Object* self, ObjectArray args) {
    DiscasterString inputFileName = args.at(0).stringValue();
    DiscasterInt skippedInitialSectors = args.at(1).intValue();
    
    int totalSize = -1;
    if (args.size() >= 3) totalSize = args.at(2).intValue();
    
    Object obj = BlobObject("addRawData");
    obj.setMember("inputFileName", inputFileName);
    obj.setMember("skippedInitialSectors", skippedInitialSectors);
    obj.setMember("totalSize", totalSize);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds audio data from an uncompressed 16-bit signed RIFF WAVE file to the
   * CD.
   * The format must be correct or an error will occur.
   *
   * arg0 inputFileName Name of the input file.
   */
  static ParseNode* addWavAudio(Object* env, Object* self, ObjectArray args) {
    DiscasterString inputFileName = args.at(0).stringValue();
    
    Object obj = BlobObject("addWavAudio");
    obj.setMember("inputFileName", inputFileName);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds an IsoFilesystem to the build list.
   *
   * arg0 isoFilesystem The IsoFilesystem.
   */
  static ParseNode* addIsoFilesystem(Object* env, Object* self, ObjectArray args) {
    Object& isoFilesystem = args.at(0);
    
    Object obj = BlobObject("addIsoFilesystem");
    obj.setMember("isoFilesystem", isoFilesystem);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds a CD mode change command to the build list.
   *
   * arg0 modestring String identifying the target mode. See addTrackStart().
   */
  static ParseNode* addModeChange(Object* env, Object* self, ObjectArray args) {
    std::string modestring = args.at(0).stringValue();
    
    Object obj = BlobObject("addModeChange");
    obj.setMember("modestring", modestring);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds a pad-to-next-sector command to the build list.
   * If the disc write position is between sectors, this will advance it
   * to the start of the next one, leaving the end of the current sector
   * zero-filled.
   * If the write position is already at a sector boundary, this does nothing.
   *
   * arg0 modestring String identifying the target mode. See addTrackStart().
   */
  static ParseNode* addPadToSectorBoundary(
      Object* env, Object* self, ObjectArray args) {
//    std::string modestring = args.at(0).stringValue();
    
    Object obj = BlobObject("addPadToSectorBoundary");
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Adds a pregap of the specified minute-second-frame size to the disc.
   * Will define track indices 00 and 01 as the start and end of the pregap,
   * respectively. If either one is already defined for the current track,
   * this will generate an error.
   * Size in sectors = ((min * 60 * 75) + (sec * 75) + frame).
   *
   * arg0 min Minutes component of pregap size.
   * arg1 sec Seconds component of pregap size.
   * arg2 frame Frames component of pregap size.
   */
  static ParseNode* addPregapMsf(
      Object* env, Object* self, ObjectArray args) {
    int lenMin = args.at(0).intValue();
    int lenSec = args.at(1).intValue();
    int lenFrame = args.at(2).intValue();
    
    Object obj = BlobObject("addPregapMsf");
    obj.setMember("lenMin", lenMin);
    obj.setMember("lenSec", lenSec);
    obj.setMember("lenFrame", lenFrame);
    addBuildCommand(self, obj);
    
    return NULL;
  }
  
  /**
   * Generates a raw disc image.
   */
  static void buildCd(Object* env, Object* self, CdBuilder& cd) {
    // retrieve the raw build list
    const Object& buildListObj = self->getMember("buildList");
    const ObjectArray& buildList = buildListObj.arrayValue();
    
    // iterate over all build commands
    for (ObjectArray::const_iterator it = buildList.begin();
         it != buildList.end();
         ++it) {
      const Object& buildCmdObj = *it;
      std::string buildCmdName = buildCmdObj.type();
      
      if (buildCmdName.compare("addTrackStart") == 0) {
        int trackNum = buildCmdObj.getMember("trackNum").intValue();
        CdModeIds::CdModeId modeId = static_cast<CdModeIds::CdModeId>
          (buildCmdObj.getMember("modeId").intValue());
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Starting track " << trackNum << std::endl;
        }
        
        cd.startNewTrack(trackNum, modeId);
      }
      else if (buildCmdName.compare("addTrackEnd") == 0) {
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Ending track " << cd.currentTrackNum() << std::endl;
        }
        
        cd.endCurrentTrack();
      }
      else if (buildCmdName.compare("addTrackIndex") == 0) {
        int indexNum = buildCmdObj.getMember("indexNum").intValue();
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Adding index point " << indexNum
                    << " to track " << cd.currentTrackNum() << std::endl;
        }
        
        cd.addTrackIndex(indexNum);
      }
      else if (buildCmdName.compare("addNullSectors") == 0) {
        int numSectors = buildCmdObj.getMember("numSectors").intValue();
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
            << "Adding " << numSectors << " null sector(s)" << std::endl;
        }
        
        cd.addNullSectors(numSectors);
        
/*        char data[] = { 't', 'e', 's', 't' };
        
        int numSectors = buildCmdObj.getMember("numSectors").intValue();
        for (int i = 0; i < numSectors; i++) {
          cd.writeData(data, sizeof(data));
        } */
        
/*        TBufStream test;
        test.open("sector5.bin");
        cd.writeData((BlackT::TByte*)(test.data().data()) + 0x10, 0x800);
        test.open("sector6.bin");
        cd.writeData((BlackT::TByte*)(test.data().data()) + 0x10, 0x800);
        
//        const char* str = "abc xyz 123 xxx";
//        cd.overwriteData(0x10, (BlackT::TByte*)str, strlen(str));
        
        TBufStream test;
        test.open("sector7.bin");
        cd.writeData((BlackT::TByte*)test.data().data() + 0x18, 0x800);
        test.open("sector8.bin");
        cd.writeData((BlackT::TByte*)test.data().data() + 0x18, 0x800); */
      }
      else if (buildCmdName.compare("addEmptySectors") == 0) {
        int numSectors = buildCmdObj.getMember("numSectors").intValue();
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
            << "Adding " << numSectors << " empty sector(s)" << std::endl;
        }
        
        cd.addEmptySectors(numSectors);
      }
      else if (buildCmdName.compare("addLabel") == 0) {
        DiscasterString labelName
          = buildCmdObj.getMember("labelName").stringValue();
        
/*        int sectorNum = cd.currentSectorNum();
        
        // if used while sector active, label applies to the *next*
        // sector. otherwise, labels will incorrectly point to the last
        // sector of the previous content instead of the start of the new.
        if (cd.sectorIsActive()) ++sectorNum;
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Adding label \"" << labelName << "\"" << std::endl;
        }
        
        self->getMember("discLabels").setMember(
            labelName, IntObject(sectorNum)); */
        
        cd.addDiscLabel(labelName);
      }
      else if (buildCmdName.compare("addRawData") == 0) {
        DiscasterString inputFileName
          = buildCmdObj.getMember("inputFileName").stringValue();
        DiscasterInt skippedInitialSectors
          = buildCmdObj.getMember("skippedInitialSectors").intValue();
        DiscasterInt totalSize
          = buildCmdObj.getMember("totalSize").intValue();
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Adding raw data from file \"" << inputFileName << "\""
                    << " (initial sectors skipped: "
                    << skippedInitialSectors
                    << ")"
                    << std::endl;
          if (totalSize != -1) {
            std::cout << "Limiting write size to " << totalSize << " sectors"
              << std::endl;
          }
        }
        
        cd.writeDataFromFileWithSkippedInitialSectors(
          inputFileName, skippedInitialSectors, totalSize);
      }
      else if (buildCmdName.compare("addWavAudio") == 0) {
        DiscasterString inputFileName
          = buildCmdObj.getMember("inputFileName").stringValue();
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Adding audio data from WAV file \""
                    << inputFileName << "\""
                    << std::endl;
        }
        
        TSoundFile sound;
        {
//          TIfstream ifs(inputFileName.c_str(), std::ios_base::binary);
          TBufStream ifs;
          ifs.open(inputFileName.c_str());
          sound.importWav(ifs);
        }
        
        TBufStream ofs;
        sound.exportRaw16BitSigned(ofs);
        
        cd.padToSectorBoundary();
        cd.writeData((TByte*)ofs.data().data(), ofs.size());
        cd.padToSectorBoundary();
        
//        cd.writeDataFromFile(inputFileName);
      }
      else if (buildCmdName.compare("addIsoFilesystem") == 0) {
        Object isoFilesystem = buildCmdObj.getMember("isoFilesystem");
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Building ISO filesystem (initial pass)"
                    << std::endl;
        }
        
        cd.addIsoFilesystem(*env, isoFilesystem);
      }
      else if (buildCmdName.compare("addModeChange") == 0) {
        std::string modestring
          = buildCmdObj.getMember("modestring").stringValue();
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Changing mode to "
                    << "\""
                    << modestring
                    << "\""
                    << std::endl;
        }
        
        cd.changeSectorMode(CdConsts::modeStringToId(modestring));
      }
      else if (buildCmdName.compare("addPadToSectorBoundary") == 0) {
        if (cd.sectorIsActive()) {
          if (config.debugOutput()) {
            std::cout << "Sector " << cd.currentSectorNum() << ": "
                      << "Padding to next sector boundary"
                      << std::endl;
          }
          
          cd.padToSectorBoundary();;
        }
        else {
          if (config.debugOutput()) {
            std::cout << "Sector " << cd.currentSectorNum() << ": "
                      << "Skipping padding (already at sector boundary)"
                      << std::endl;
          }
        }
      }
      else if (buildCmdName.compare("addPregapMsf") == 0) {
        int lenMin = buildCmdObj.getMemberInt("lenMin");
        int lenSec = buildCmdObj.getMemberInt("lenSec");
        int lenFrame = buildCmdObj.getMemberInt("lenFrame");
        
        CdMsf msf;
        msf.min = lenMin;
        msf.sec = lenSec;
        msf.frame = lenFrame;
        int sectorCount = msf.toSectorNum();
        
        if (config.debugOutput()) {
          std::cout << "Sector " << cd.currentSectorNum() << ": "
                    << "Adding pregap of "
                    << sectorCount
                    << " sectors"
                    << std::endl;
        }
        
        cd.addPregap(sectorCount);
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "buildCd(Object*,Object*,CdBuilder&)",
                                std::string("Unknown build command: ")
                                  + buildCmdName);
      }
      
    }
    
//    bool disableEccEdcCalc = self->getMemberInt("disableEccEdcCalculation");
    bool disableEccCalc = self->getMemberInt("disableEccCalculation");
//    cd.setDisableEccEdcCalculation(disableEccEdcCalc);
    cd.setDisableEccCalculation(disableEccCalc);
    
    cd.finishCd();
  }
  
  /**
   * Generates the disc image and exports it to BIN/CUE format.
   *
   * arg0 baseExportName Base filename.
   *                     This is the name of the exported .BIN/.CUE files
   *                     with no extension, e.g. "outfolder/image" will generate
   *                     the files "outfolder/image.bin" and
   *                     "outfolder/image.cue".
   */
  static ParseNode* exportBinCue(Object* env, Object* self, ObjectArray args) {
    // base export filename with no extension
    DiscasterString baseExportName = args.at(0).stringValue();
    
    DiscasterString exportBinName = baseExportName + ".bin";
    DiscasterString exportCueName = baseExportName + ".cue";
    
    TFileManip::createDirectoryForFile(exportBinName);
        
    if (config.debugOutput()) {
      std::cout << "Preparing to build disc image \""
        << exportBinName << "\"" << std::endl;
    }
    
    std::fstream binStream(exportBinName.c_str(),
                           std::ios_base::binary
                             | std::ios_base::trunc
                             | std::ios_base::in
                             | std::ios_base::out);
    CdBuilder cd(binStream);
    
    buildCd(env, self, cd);
    
    // export cue
    cd.exportCueSheet(exportCueName, exportBinName);
//    {
//      std::ofstream ofs(exportCueName.c_str(), ios_base::trunc);
//      cd.exportCueSheet(ofs, baseFile + ".bin");
//    }
    
    return NULL;
  }
  
  CdImage::CdImage()
    : BlobObject() {
    //============================
    // Set up functions
    //============================
    
    addFunction("addTrackStart", addTrackStart);
    addFunction("addTrackEnd", addTrackEnd);
    addFunction("exportBinCue", exportBinCue);
    addFunction("addTrackIndex", addTrackIndex);
    addFunction("addNullSectors", addNullSectors);
    addFunction("addEmptySectors", addEmptySectors);
    addFunction("addLabel", addLabel);
    addFunction("addRawData", addRawData);
    addFunction("addRawDataWithSkippedInitialSectors",
                addRawDataWithSkippedInitialSectors);
    addFunction("addIsoFilesystem", addIsoFilesystem);
    addFunction("addModeChange", addModeChange);
    addFunction("addWavAudio", addWavAudio);
    addFunction("addPadToSectorBoundary", addPadToSectorBoundary);
    addFunction("addPregapMsf", addPregapMsf);
    
    //============================
    // Set up members
    //============================
    
    // array of build commands, executed sequentially upon disc export
    setMember("buildList", ArrayObject());
    // user-defined disc sector labels
//    setMember("discLabels", BlobObject("disc label list"));
    
    // ECC/EDC calculation enable/disable
//    setMember("disableEccEdcCalculation", IntObject(0));
    setMember("disableEccCalculation", IntObject(0));
  }


}
