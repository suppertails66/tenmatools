#ifndef MADOU1PCESCRIPTREADER_H
#define MADOU1PCESCRIPTREADER_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Pce {


class Madou1PceScriptReader {
public:
//  typedef std::map<int, int> CharSizeTable;

  struct ResultString {
    std::string str;
    int srcOffset;
    int srcPointer;
//    int srcOpOffset;
//    int index;
//    std::string name;
    int width;
    std::vector<int> indices;
  };
  
  typedef std::vector<ResultString> ResultCollection;
  typedef std::map<int, ResultCollection> RegionToResultMap;

  Madou1PceScriptReader(BlackT::TStream& src__,
//                  BlackT::TStream& dst__,
//                  NesRom& dst__,
                  RegionToResultMap& dst__,
                  const BlackT::TThingyTable& thingy__);
  
  bool operator()();
protected:

  BlackT::TStream& src;
  RegionToResultMap& dst;
  BlackT::TThingyTable thingy;
  int lineNum;
  bool breakTriggered;
  int regionNum;
  
  BlackT::TBufStream currentScriptBuffer;
  int currentScriptSrcOffset;
  int currentScriptSrcPointer;
//  int currentScriptSrcOpOffset;
//  int currentScriptIndex;
//  std::string currentScriptName;
  int currentScriptWidth;
  std::vector<int> currentScriptIndices;
  
  void outputNextSymbol(BlackT::TStream& ifs);
  
//  bool checkSymbol(BlackT::TStream& ifs, std::string& symbol);
  
  void flushActiveScript();
  void resetScriptBuffer();
  
  void processDirective(BlackT::TStream& ifs);
  void processLoadTable(BlackT::TStream& ifs);
  void processStartMsg(BlackT::TStream& ifs);
  void processEndMsg(BlackT::TStream& ifs);
  void processIncBin(BlackT::TStream& ifs);
  void processBreak(BlackT::TStream& ifs);
  void processStartRegion(BlackT::TStream& ifs);
  void processEndRegion(BlackT::TStream& ifs);
  void processSetIndexList(BlackT::TStream& ifs);
  
  void loadThingy(const BlackT::TThingyTable& thingy__);
  
};


}


#endif
