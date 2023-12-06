#ifndef DISCASTERCDBUILDER_H
#define DISCASTERCDBUILDER_H


#include "discaster/CdConsts.h"
#include "classes/IsoFilesystem.h"
#include "classes/IsoPrimaryVolumeDescriptor.h"
#include "classes/IsoDescriptorSetTerminator.h"
#include "classes/IsoPathTable.h"
#include "util/TByte.h"
#include "util/TStream.h"
#include "util/TGaloisField256.h"
#include "util/TGaloisField256Cd.h"
#include "util/TEdcCalc.h"
#include "util/EndiannessType.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace Discaster {
  
  typedef std::map<int, int> IndexSectorMap;

  struct TrackEntry {
    TrackEntry();
    int startSector;
    int sectorSize;
    CdModeIds::CdModeId mode;
    IndexSectorMap indices;
  };
  
  typedef std::map<int, TrackEntry> TrackSectorMap;
  
  namespace XaSubheaderFlagModes {
    enum XaSubheaderFlagMode {
      none,
      lastSector,
      everySector
    };
  };

  class CdBuilder {
  public:
    CdBuilder(std::iostream& cdbuf__);
    
    /**
     * Returns current track number, or -1 if no track active.
     */
    int currentTrackNum() const;
    
    /**
     * Returns current sector number.
     */
    int currentSectorNum() const;
    
    /**
     * Returns true if a sector is currently active (disc write position
     * is not at a sector boundary).
     */
    bool sectorIsActive() const;
    
    /**
     * Returns true if a track is currently active.
     */
    bool trackIsActive() const;
    
    /**
     * Returns current byte position (disc-absolute, not including initial
     * pregap).
     * NOTE: If currently between sectors, the value returned is based
     *       based on the assumption that the next sector will be of the
     *       same mode as the previous.
     *       For example, if we've written exactly one sector's worth of
     *       data in MODE1, then this will return 0x940 (the start of the
     *       Data Area for the second sector).
     *       But if the mode is changed to AUDIO, this function will then
     *       return 0x930.
     *       Be careful to make any mode changes before calling this function,
     *       or you may get unexpected results.
     */
    int currentByteNum() const;
    
    /**
     * Starts a new CD track.
     * Ends any other currently active track.
     *
     * @arg trackNum Track number.
     * @arg modeId Default mode of the track.
     *             May be overriden within the same track by subsequent
     *             commands.
     */
    void startNewTrack(int trackNum, CdModeIds::CdModeId modeId);
    
    /**
     * Ends the current CD track.
     */
    void endCurrentTrack();
    
    /**
     * Adds an index point to the current CD track at the current sector.
     * Calling this without an active track is an error.
     *
     * @arg indexNum Index number.
     */
    void addTrackIndex(int indexNum);
    
    /**
     * Writes raw data to disc in the current mode at the current write
     * position.
     *
     * @arg src Source data.
     * @arg len Length of source data in bytes.
     */
    void writeData(const BlackT::TByte* data, int len);
    
    /**
     * Writes raw data to disc in the current mode at the current write
     * position, sourced from a file on disk.
     *
     * @arg inputFileName Name of file containing source data.
     */
    void writeDataFromFile(std::string inputFileName);
    
    /**
     * Writes raw data to disc in the current mode at the current write
     * position, sourced from a file on disk, skipping the given number
     * of initial sectors at the start of the file.
     *
     * @arg inputFileName Name of file containing source data.
     * @arg skippedInitialSectors Number of initial sectors to skip.
     * @arg totalSize Total size of data to add in sectors.
     *                If -1, add through end of input file.
     */
    void writeDataFromFileWithSkippedInitialSectors(
      std::string inputFileName, int skippedInitialSectors,
      int totalSize = -1);
    
    /**
     * Writes raw data to disc in the current mode at the current write
     * position, sourced from a TStream.
     *
     * @arg ifs Input stream.
     */
    void writeData(BlackT::TStream& ifs);
    
    /**
     * Writes raw 0x930-byte sectors to disc in the current mode at the
     * current write position, sourced from a TStream.
     * The input stream must consist of 0x930-byte sectors that have had
     * all header fields other than the disc position and (possibly) ECC/EDC
     * data filled in.
     *
     * @arg ifs Input stream.
     */
    void writeRawSectorData(BlackT::TStream& ifs);
    
    /**
     * Advances write position to next sector boundary.
     * If already at a sector boundary, does nothing.
     * Remaining bytes in the Data Area will be zero-filled.
     */
    void padToSectorBoundary();
    
    /**
     * Adds totally null (zero-filled) sectors to the CD.
     * If a sector is already active, it will be completed (not counting as one
     * of the added null sectors), then the null sector(s) will be added.
     *
     * @arg numSectors Number of sectors to add.
     */
    void addNullSectors(int numSectors);
    
    /**
     * Adds empty (headered, but zero-filled) sectors to the CD.
     * If a sector is already active, it will be completed (not counting as one
     * of the added empty sectors), then the empty sector(s) will be added.
     *
     * @arg numSectors Number of sectors to add.
     */
    void addEmptySectors(int numSectors);
    
    /**
     * Adds empty (Data Area is zero-filled) sectors to the CD.
     * If a sector is already active, it will be completed (not counting as one
     * of the added blank sectors), then the blank sector(s) will be added.
     * Differs from addNullSectors in that the sectors are not all zeroes, but
     * are recorded in the current CD mode (including sync header, etc.)
     *
     * @arg numSectors Number of sectors to add.
     */
    void addPadSectors(int numSectors);
    
    /**
     * Uses an IsoFilesystem object to construct an ISO filesystem on the CD.
     *
     * @arg environment The scripting environment.
     * @arg isoFilesystem The filesystem object.
     */
    void addIsoFilesystem(Object& env, const Object& iso);
    
    /**
     * Adds a sector label at the current position.
     *
     * @arg name Name of the label.
     */
    void addDiscLabel(std::string name);
    
    /**
     * Adds a pregap of the specified size at the current position.
     * @see CdImage::addPregapMsf()
     *
     * @arg sizeInSectors Size of the pregap in sectors.
     */
    void addPregap(int sizeInSectors);
    
    /**
     * Changes sector mode.
     * Will pad to nearest sector boundary first.
     *
     * @arg name Name of the label.
     */
    void changeSectorMode(CdModeIds::CdModeId modeId);
    
    /**
     * Sets whether or not ECC and EDC calculations are disabled.
     *
     * @arg disableEccEdcCalculation__ New state.
     */
    void setDisableEccCalculation(bool disableEccCalculation__);
    
    /**
     * Performs final calculations on CD.
     * This will compute and fill in the ECC and/or EDC of all sectors that
     * require them.
     */
    void finishCd(Object* env = NULL);
    
    /**
     * Exports a CUE sheet for the disc.
     *
     * @arg exportCueName Target path for the output CUE file.
     * @arg exportBinName Location of the BIN associated with the CUE.
     *                    NOTE: This is assumed to appear in the same directory
     *                    as the associated CUE, such that the CUE can reference
     *                    the BIN by filename alone (no directories in the
     *                    path).
     */
    void exportCueSheet(std::string exportCueName,
                        std::string exportBinName) const;
    
    void setXaEorFlagMode(XaSubheaderFlagModes::XaSubheaderFlagMode mode);
    void setXaEofFlagMode(XaSubheaderFlagModes::XaSubheaderFlagMode mode);
    
  protected:
//    std::string outputFileName_;
    
    /**
     * Bidirectional stream containing current CD state.
     */
    std::iostream& cdbuf_;
    
    /**
     * The position of offset zero within cdbuf_.
     * If cdbuf_ was passed to us at a nonzero position, this tells us what
     * the base position is.
     */
    int discStreamBasePos_;
    
    /**
     * Current write position, in bytes, from start of disc.
     */
    int writePos_;
    
    /**
     * Current write position, in sectors, from start of disc.
     */
    int currentSectorNum_;
    
    /**
     * The byte number at which the current sector's Data Area ends.
     * Once writePos_ reaches this value, the sector should be ended.
     * Set to -1 if "between" sectors (old sector filled, new one not yet begun.
     */
    int dataAreaEndPos_;
    
    /**
     * Track index.
     * Associates each track number with an initial sector number.
     * Sector number should correspond to start of track pregap (if one exists).
     */
    TrackSectorMap trackIndex_;
    
    /**
     * Active track number (-1 if no track active).
     */
    int currentTrackNum_;
    
    /**
     * Current mode ID as specified by active track.
     * Default for writes of unspecified sector type.
     */
    CdModeIds::CdModeId currentSectorMode_;

    /**
     * Maximum number of sectors that can be written to the disc.
     */
    int maxSectorCount_;

    /**
     * Size of lead-in pregap (not part of disc image, but required for
     * certain calculations).
     */
    int initialPregapSectors_;
    
    /**
     * If set, ECC/EDC calculations are disabled.
     */
    bool disableEccCalculation_;

    /**
     * Array in which each byte position corresponds to that sector number
     * on the disc and indicates the mode in which the sector was recorded
     * (per the CdModeId enum).
     * Unrecorded sectors are filled with 0xFF.
     */
//    BlackT::TByte sectorModeMap_[CdConsts::cdCapacityInSectors];
    BlackT::TByte* sectorModeMap_;

    /**
     * Zero-filled array sized to a sector for speedy stream fills when
     * starting a new sector.
     */
    BlackT::TByte blankSectorArray_[CdConsts::physicalSectorSize];
    
    /**
     * Sector sync data.
     * Present at the start of most sector types.
     */
    const static BlackT::TByte sectorSyncField_[];
    
    /**
     * Temporary storage for "actual" write state when performing overwrites.
     */
     
    CdModeIds::CdModeId old_currentSectorMode_;
    int old_writePos_;
    int old_currentSectorNum_;
    int old_dataAreaEndPos_;
    unsigned int old_cdbufPutPos_;
    
    /**
     * Internal state flag set when writing a data file to disc.
     */
//    bool isWritingFile_;
    
    /**
     * XA subheader flag.
     * If set, whenever a MODE2 sector is created, the end-of-record flag in
     * the subheader will be set.
     * Other sector types are unaffected.
     */
//    bool xaSubheaderIsAlwaysEor_;
    XaSubheaderFlagModes::XaSubheaderFlagMode xaEorFlagMode_;
    
    /**
     * XA subheader flag.
     * If set, whenever a MODE2 sector is created, and that sector would be
     * the last of a writeData() or writeDataFromFile() command, it will be
     * flagged as end-of-record.
     * Other sector types are unaffected.
     */
//    bool xaSubheaderEndsInEor_;
    
    /**
     * XA subheader flag.
     * If set, whenever a MODE2 sector is created, and that sector would be
     * the last of a writeData() or writeDataFromFile() command, it will be
     * flagged as end-of-file.
     * Other sector types are unaffected.
     */
//    bool xaSubheaderEndsInEof_;
    XaSubheaderFlagModes::XaSubheaderFlagMode xaEofFlagMode_;
    
    /**
     * Default XA subheader values.
     * These values are written to the XA subheader when they are not
     * provided from some other source (e.g. a file listing).
     */
    
    int xaFileNumber_;
    int xaChannelNumber_;
    
    // note: EOR, EOF, and Form2 handled specially
    bool xaVideoFlag_;
    bool xaAudioFlag_;
    bool xaDataFlag_;
    bool xaTriggerFlag_;
    bool xaRealTimeFlag_;
    
    /**
     * Internal state flag for EOF flag in new MODE2 sectors.
     */
    bool newMode2SectorsAreEof_;
    
    /**
     * Internal state flag for EOR flag in new MODE2 sectors.
     */
    bool newMode2SectorsAreEor_;
    
    //==================================================================
    // containers for build data
    //==================================================================
    
    typedef std::vector<Object> IsoFilesystemCollection;
    typedef std::vector<Object> IsoPrimaryVolumeDescriptorCollection;
    
    struct IsoWithAssociates {
      Object iso;
      IsoPrimaryVolumeDescriptorCollection primaryVolumeDescriptors;
      Object typeLPathTable;
      Object typeLPathTableCopy;
      Object typeMPathTable;
      Object typeMPathTableCopy;
    };
    
    typedef std::vector<IsoWithAssociates>
      IsoWithAssociatesCollection;

    /**
     * Record of IsoFilesystems that have been written to the disc.
     * Also stores new data like filesystem and file locations so they
     * can be updated later.
     */
//    IsoFilesystemCollection isoFilesystems_;
    
//    IsoPrimaryVolumeDescriptorCollection isoPrimaryVolumeDescriptors_;
    
    IsoWithAssociatesCollection isosWithAssociates_;
    
    Object discLabels_;
    
    //==================================================================
    // functions
    //==================================================================

    static int byteToSectorPos(int bytePos);
    static int sectorToBytePos(int bytePos);
    static int byteToSectorLocalPos(int bytePos);
//    static int sectorDataAreaStartPos(CdModeIds::CdModeId modeId);
//    static int sectorDataAreaEndPos(CdModeIds::CdModeId modeId);
    
//    int currentSectorNum() const;
    
    /**
     * Overwrites the data at a previously written position on the disc.
     * The data is written to the Data Area of the sector according
     * to its recorded sector type (Mode 1, Mode 2 of either form, etc.).
     * If the amount of data to be copied in extends past the end of the
     * initial sector's Data Area, it will carry over into the beginning of
     * the next sector's Data Area, and so on through sequential sectors.
     *
     * Only previously created sectors can be overwritten. An overwrite
     * that extends into an undefined sector will generate an error.
     * For adding new sectors to the disc, other write functions must be used.
     *
     * After the write, the disc write offset is reset to its position at
     * the start of the function (i.e. subsequent writes will continue from
     * the previous position, as though the overwrite never happened).
     *
     * The sector mode must be the same throughout the entire area targeted
     * by the overwrite.
     *
     * @arg offset The absolute offset, in bytes, at which to begin the
     *             overwrite.
     *             This MUST be positioned within the Data Area of the
     *             corresponding sector, and is an error if not.
     * @arg src Source data.
     * @arg len Length of source data in bytes.
     */
    void overwriteData(int offset, const BlackT::TByte* src, int len);
    
    /**
     * Prepare the current sector for writing in the specified mode.
     * Call whenever preparing to write data to a new sector, with the
     * writePos_ positioned at the start of the new sector.
     * Writes any relevant header fields, moves the write position to the
     * start of the Data Area, and sets dataAreaEndPos_ according to the
     * size and position of the Data Area.
     */
    void prepNextSector(CdModeIds::CdModeId modeId);
    
    /**
     * Finish the current sector.
     * Advances the write position to the start of the next sector,
     * leaving any remaining data in the old sector zero-filled.
     */
    void finishSector();
    
    /**
     * Compute the cross-interleaved Reed-Solomon code of a 0x810-byte piece
     * of data.
     * 
     * @arg src Source data. Should be exactly 0x810 bytes in size.
     * @arg dst Destination. Should be at least 0x114 bytes in size.
     * @arg field The Galois field over which to compute the CIRC.
     * @arg generator The generator polynomial for the CIRC.
     */
//    void computeSectorEcc(const BlackT::TByte* src,
//                           BlackT::TByte* dst,
//                           BlackT::TGaloisField256& field,
//                           BlackT::TGaloisField256Polynomial& generator) const;
    void computeSectorEcc(const BlackT::TByte* src,
                           BlackT::TByte* dst,
                           BlackT::TGaloisField256Cd& field) const;
    
    /**
     * Compute the EDC (CRC32) of a piece of data.
     * 
     * @arg src Source data.
     * @arg len Length of source data in bytes.
     * @return EDC of the source data.
     */
    unsigned int computeEdc(const BlackT::TByte* src, int len) const;
    
    CdModeIds::CdModeId getSectorMode(int sectorNum) const;
    
    void beginOverwrite(int offset);
    
    void beginOverwriteAtSector(int sectorNum);
    
    void endOverwrite();
    
    int remainingBytesInSector() const;
    
    /**
     * Writes to dst directory records for all files and folders located in
     * directory "path" within the given IsoFilesystem's file listing.
     * Non-recursive (subdirectories are not explored).
     * Ordering is conformant to ISO specifications.
     */
    void writeDirectoryDescriptor(Object& env, Object& iso,
                                std::string path,
                                CdFormatIds::CdFormatId cdFormatId);
    
    void writeDirectoryDescriptor(Object& env, Object& iso,
                                Object* targetDirectory,
                              std::string path,
                                CdFormatIds::CdFormatId cdFormatId);
    
    int getLogicalBlockOffset(
      Object* env, Object* iso, int sectorOffset);
    
    int getLogicalBlockSize(
      Object* env, Object* iso, int sectorSize);
    
    void resolveDiscReferencesStep(
      Object* env, Object* iso, Object* directoryListing);
    
    void rewriteDiscReferencesStep(
      Object* env, Object* iso, Object* directoryListing,
      std::string path);
      
    Object generatePathTable(Object& env, Object& iso);
      
    void writePathTable(Object& env, Object& pathTable,
                         BlackT::EndiannessTypes::EndiannessType endianness);
      
    void updatePathTable(Object& env, Object& iso, Object& pathTable,
                         BlackT::EndiannessTypes::EndiannessType endianness);
    
    /**
     * Writes raw data to disc in the current mode at the current write
     * position, sourced from a TStream, and uses the specified XA subheader
     * flags for the write.
     *
     * @arg ifs Input stream.
     * @arg xaEorMode Mode for EOR flag.
     * @arg xaEofMode Mode for EOF flag.
     */
    void writeDataWithXaFlags(BlackT::TStream& ifs,
                   XaSubheaderFlagModes::XaSubheaderFlagMode xaEorMode
                      = XaSubheaderFlagModes::none,
                   XaSubheaderFlagModes::XaSubheaderFlagMode xaEofMode
                      = XaSubheaderFlagModes::none);
    
    void writeDataFromFileWithXaFlags(std::string inputFileName,
                   XaSubheaderFlagModes::XaSubheaderFlagMode xaEorMode
                      = XaSubheaderFlagModes::none,
                   XaSubheaderFlagModes::XaSubheaderFlagMode xaEofMode
                      = XaSubheaderFlagModes::none);
    
  };


}


#endif
