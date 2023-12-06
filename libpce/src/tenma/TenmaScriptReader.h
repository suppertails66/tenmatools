#ifndef TENMASCRIPTREADER_H
#define TENMASCRIPTREADER_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include "util/TStringConversion.h"
#include "util/TFreeSpace.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Pce {


class TenmaScriptReader {
public:
//  typedef std::map<int, int> CharSizeTable;

  struct ResultString {
    ResultString();
    
    std::string id;
    std::string str;
    int originalOffset;
    int originalSize;
    bool initialized;
    bool autoStarted;
    bool isLiteral;
    bool isNotCompressible;
    std::map<std::string, std::string> properties;
    std::vector<int> pointerRefs;
    
    bool hasProperty(std::string name) {
      std::map<std::string, std::string>::iterator findIt
        = properties.find(name);
      if (findIt == properties.end()) return false;
      return true;
    }
    
    bool propertyIsTrue(std::string name) {
      std::map<std::string, std::string>::iterator findIt
        = properties.find(name);
      if (findIt == properties.end()) return false;
      return BlackT::TStringConversion::stringToInt(findIt->second) != 0;
    }
  };
  
  typedef std::vector<ResultString> ResultCollection;

  enum RegionType {
    regionType_none,
    regionType_area,
    regionType_adv
  };
  
  struct ResultRegion {
    ResultRegion();
    
    ResultCollection strings;
    
    std::string filename;
    RegionType regionType;
    int soundCount;
    int choiceCount;
    std::map<std::string, std::string> properties;
    BlackT::TFreeSpace freeSpace;
    
    bool hasProperty(std::string name) {
      std::map<std::string, std::string>::iterator findIt
        = properties.find(name);
      if (findIt == properties.end()) return false;
      return true;
    }
    
    bool propertyIsTrue(std::string name) {
      std::map<std::string, std::string>::iterator findIt
        = properties.find(name);
      if (findIt == properties.end()) return false;
      return BlackT::TStringConversion::stringToInt(findIt->second) != 0;
    }
  };
  
  typedef std::map<std::string, ResultRegion> NameToRegionMap;

  TenmaScriptReader(BlackT::TStream& src__,
                  NameToRegionMap& dst__,
                  const BlackT::TThingyTable& thingy__);
  
  bool operator()();
protected:

  BlackT::TStream& src;
  NameToRegionMap& dst;
  BlackT::TThingyTable thingy;
  int lineNum;
  bool breakTriggered;
  std::string regionName;
  
  BlackT::TBufStream currentScriptBuffer;
  bool sceneModeOn;
  bool stringIsActive;
  bool italicModeOn;
  double lastWaitTime;
  double timeScale;
  ResultString currentResult;
//  bool sceneModeOn;
  
  const static char literalOpenSymbol = '<';
  const static char literalCloseSymbol = '>';
  
  void outputNextSymbol(BlackT::TStream& ifs);
  
//  bool checkSymbol(BlackT::TStream& ifs, std::string& symbol);
  
  void flushActiveScript();
  void resetScriptBuffer();
  
  void terminateCurrentStringIfActive();
  void outputTerminateCommand();
  void outputOffCommand();
  void outputWaitCommand(double secTime);
  void outputSlotCommand(int slotNum);
  void outputPalCommand(int palNum);
  void outputPointsCommand(double point1time, double point2time);
  void outputAlignModeCommand(int alignMode);
  void outputSyncCommand(int modeId, int targetValue, double newTime);
  void outputStartStreamCommand(int streamIndex, int scriptId);
  
  void processDirective(BlackT::TStream& ifs);
  void processLoadTable(BlackT::TStream& ifs);
  void processStartMsg(BlackT::TStream& ifs);
  void processEndMsg(BlackT::TStream& ifs);
  void processIncBin(BlackT::TStream& ifs);
  void processBreak(BlackT::TStream& ifs);
  void processStartRegion(BlackT::TStream& ifs);
  void processEndRegion(BlackT::TStream& ifs);
//  void processSetIndexList(BlackT::TStream& ifs);
  
  void loadThingy(const BlackT::TThingyTable& thingy__);

  double matchTime(BlackT::TStream& ifs);
  
};


}


#endif
