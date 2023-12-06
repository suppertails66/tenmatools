#include "discaster/CdConsts.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"

using namespace BlackT;

namespace Discaster {

  
  static void decapitalize(std::string& str) {
    for (unsigned int i = 0; i < str.size(); i++) {
      str[i] = tolower(str[i]);
    }
  }
  
  CdModeIds::CdModeId CdConsts::modeStringToId(std::string modestring) {
    std::string modestringOrig = modestring;
    
    decapitalize(modestring);
    
    if (modestring.compare("audio") == 0) {
      return CdModeIds::audio;
    }
    else if (modestring.compare("mode1") == 0) {
      return CdModeIds::mode1;
    }
    else if (modestring.compare("mode2form1") == 0) {
      return CdModeIds::mode2form1;
    }
    else if (modestring.compare("mode2form2") == 0) {
      return CdModeIds::mode2form2;
    }
    else if (modestring.compare("null") == 0) {
      return CdModeIds::null;
    }
    else if (modestring.compare("mode0") == 0) {
      return CdModeIds::mode0;
    }
    else if (modestring.compare("mode1raw") == 0) {
      return CdModeIds::mode1raw;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "modeStringToId(std::string)",
                              std::string("Unknown CD mode: ")
                                + modestringOrig);
    }
  }
  
  CdModeIds::CdModeId CdConsts::modeDataToId(
      unsigned char modeByte, unsigned char submodeByte) {
/*    int newSectorMainMode = (unsigned char)(buffer[0xF]);
    switch (newSectorMainMode) {
    case 0: // empty
    {
      currentSectorMode_ = CdModeIds::mode0;
    }
      break;
    case 1: // cd-rom
    {
      currentSectorMode_ = CdModeIds::mode1;
    }
      break;
    case 2: // cd-xa
    {
      // check submode for form1/form2
      
      // verify submodes match
      int submode = (unsigned char)(buffer[0x12]);
      int submode2 = (unsigned char)(buffer[0x16]);
      if (submode != submode2) {
        throw TGenericException(T_SRCANDLINE,
                                "CdBuilder::writeRawSectorData()",
                                std::string("Mismatched raw-sector")
                                  + " XA file submode bytes: "
                                  + TStringConversion
                                      ::toString(submode)
                                  + ", "
                                  + TStringConversion
                                      ::toString(submode2));
      }
      
      if ((submode & 0x20) == 0) {
        currentSectorMode_ = CdModeIds::mode2form1;
      }
      else {
        currentSectorMode_ = CdModeIds::mode2form2;
      }
    }
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "CdBuilder::writeRawSectorData()",
                              std::string("Unknown newSectorMainMode: ")
                                + TStringConversion
                                    ::toString(newSectorMainMode));
      break;
    } */
    
    switch (modeByte) {
    case 0: // empty
    {
      return CdModeIds::mode0;
    }
      break;
    case 1: // cd-rom
    {
      return CdModeIds::mode1;
    }
      break;
    case 2: // cd-xa
    {
      // check submode for form1/form2
      
      if ((submodeByte & 0x20) == 0) {
        return CdModeIds::mode2form1;
      }
      else {
        return CdModeIds::mode2form2;
      }
    }
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "CdConsts::modeDataToId()",
                              std::string("Unknown modeByte: ")
                                + TStringConversion
                                    ::toString(modeByte));
      break;
    }
  }
  
  std::string CdConsts::idToModeString(CdModeIds::CdModeId modeId) {
    switch (modeId) {
    case CdModeIds::audio:
      return "audio";
      break;
    case CdModeIds::mode1:
      return "mode1";
      break;
    case CdModeIds::mode2form1:
      return "mode2form1";
      break;
    case CdModeIds::mode2form2:
      return "mode2form2";
      break;
    case CdModeIds::null:
      return "null";
      break;
    case CdModeIds::mode0:
      return "mode0";
      break;
    case CdModeIds::mode1raw:
      return "mode1raw";
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "idToModeString()",
                              std::string("Unknown CD mode ID: ")
                                + TStringConversion::toString(modeId));
      break;
    }
  }
  
  std::string CdConsts::idToCueStyleModeString(CdModeIds::CdModeId modeId) {
    switch (modeId) {
    case CdModeIds::audio:
      return "AUDIO";
      break;
    case CdModeIds::mode1:
      return "MODE1/2048";
      break;
    case CdModeIds::mode2form1:
      return "MODE2/2336";
      break;
    case CdModeIds::mode2form2:
      return "MODE2/2352";
      break;
//    case CdModeIds::null:
//      return "null";
//      break;
//    case CdModeIds::mode0:
//      return "mode0";
//      break;
    case CdModeIds::mode1raw:
      return "MODE1/2352";
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "idToCueStyleModeString()",
                              std::string("Unknown or illegal CD mode ID: ")
                                + TStringConversion::toString(modeId));
      break;
    }
  }
  
  CdFormatIds::CdFormatId CdConsts::formatStringToId(std::string str) {
    std::string strOrig = str;
    decapitalize(str);
    
    if (str.compare("cdrom") == 0) {
      return CdFormatIds::cdrom;
    }
    else if (str.compare("cdxa") == 0) {
      return CdFormatIds::cdxa;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "formatStringToId(std::string)",
                              std::string("Unknown CD format: ")
                                + strOrig);
    }
  }
  
  std::string CdConsts::idToFormatString(CdFormatIds::CdFormatId modeId) {
    switch (modeId) {
    case CdFormatIds::cdrom:
      return "cdrom";
      break;
    case CdFormatIds::cdxa:
      return "cdxa";
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "idToFormatString()",
                              std::string("Unknown CD format ID: ")
                                + TStringConversion::toString(modeId));
      break;
    }
  }
  
  int CdConsts::sectorDataAreaStartPos(CdModeIds::CdModeId modeId) {
    switch (modeId) {
    case CdModeIds::mode1:
      return mode1DataAreaStartOffset;
      break;
    case CdModeIds::mode2form1:
    case CdModeIds::mode2form2:
      return mode2DataAreaStartOffset;
      break;
    default:
      return 0;
      break;
    }
  }
  
  int CdConsts::sectorDataAreaEndPos(CdModeIds::CdModeId modeId) {
    switch (modeId) {
    case CdModeIds::mode1:
      return mode1DataAreaEndOffset;
      break;
    case CdModeIds::mode2form1:
      return mode2Form1DataAreaEndOffset;
      break;
    case CdModeIds::mode2form2:
      return mode2Form2DataAreaEndOffset;
      break;
    default:
      return physicalSectorSize;
      break;
    }
  }
  
  int CdConsts::sectorDataAreaSize(CdModeIds::CdModeId modeId) {
    return (sectorDataAreaEndPos(modeId) - sectorDataAreaStartPos(modeId));
  }


}
