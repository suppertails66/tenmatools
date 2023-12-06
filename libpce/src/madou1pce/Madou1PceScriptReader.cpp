#include "madou1pce/Madou1PceScriptReader.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include "exception/TException.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>

using namespace BlackT;

namespace Pce {


//const static int scriptBufferCapacity = 0x10000;
const static int literalSize = 5;

Madou1PceScriptReader::Madou1PceScriptReader(
                  BlackT::TStream& src__,
                  RegionToResultMap& dst__,
                  const BlackT::TThingyTable& thingy__)
  : src(src__),
    dst(dst__),
    thingy(thingy__),
    lineNum(0),
    breakTriggered(false),
    regionNum(0),
    currentScriptWidth(0),
    currentScriptBuffer() {
  loadThingy(thingy__);
//  spaceOfs.open((outprefix + "msg_space.txt").c_str());
//  indexOfs.open((outprefix + "msg_index.txt").c_str());
  resetScriptBuffer();
}

bool Madou1PceScriptReader::operator()() {
  try {
    while (!src.eof()) {
      std::string line;
      src.getLine(line);
      ++lineNum;
      
  //    std::cerr << lineNum << std::endl;
      if (line.size() <= 0) continue;
      
      // discard lines containing only ASCII spaces and tabs
  //    bool onlySpace = true;
  //    for (int i = 0; i < line.size(); i++) {
  //      if ((line[i] != ' ')
  //          && (line[i] != '\t')) {
  //        onlySpace = false;
  //        break;
  //      }
  //    }
  //    if (onlySpace) continue;
      
      TBufStream ifs(line.size());
      ifs.write(line.c_str(), line.size());
      ifs.seek(0);
      
      // check for special stuff
      if (ifs.peek() == '#') {
        // directives
        ifs.get();
        processDirective(ifs);
        
        if (breakTriggered) {
          breakTriggered = false;
          return false;
        }
        
        continue;
      }
      
      while (!ifs.eof()) {
        // check for comments
        if ((ifs.remaining() >= 2)
            && (ifs.peek() == '/')) {
          ifs.get();
          if (ifs.peek() == '/') break;
          else ifs.unget();
        }
        
        outputNextSymbol(ifs);
      }
    }
    
    if (currentScriptBuffer.size() > 0) {
      flushActiveScript();
    }
    
    return true;
  }
  catch (TGenericException& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.problem() << std::endl;
    throw e;
  }
  catch (TException& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  }
  catch (std::exception& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  }
}
  
void Madou1PceScriptReader::loadThingy(const BlackT::TThingyTable& thingy__) {
  thingy = thingy__;
}
  
void Madou1PceScriptReader::outputNextSymbol(TStream& ifs) {
  // literal value
  if ((ifs.remaining() >= literalSize)
      && (ifs.peek() == '<')) {
    int pos = ifs.tell();
    
    ifs.get();
    if (ifs.peek() == '$') {
      ifs.get();
      std::string valuestr = "0x";
//      valuestr += ifs.get();
//      valuestr += ifs.get();
      valuestr += ifs.get();
      valuestr += ifs.get();
      
      if (ifs.peek() == '>') {
        ifs.get();
        int value = TStringConversion::stringToInt(valuestr);
        
//        currentScriptBuffer.writeu16be(value);
        currentScriptBuffer.writeu8(value);

        return;
      }
    }
    
    // not a literal value
    ifs.seek(pos);
  }
  
  TThingyTable::MatchResult result;
  result = thingy.matchTableEntry(ifs);
  
  if (result.id != -1) {
//    std::cerr << std::dec << lineNum << " " << std::hex << result.id << " " << result.size << std::endl;
  
    int symbolSize;
    if (result.id <= 0xFF) symbolSize = 1;
    else if (result.id <= 0xFFFF) symbolSize = 2;
    else if (result.id <= 0xFFFFFF) symbolSize = 3;
    else symbolSize = 4;
    
    currentScriptBuffer.writeInt(result.id, symbolSize,
      EndiannessTypes::big, SignednessTypes::nosign);
    
    return;
  }
  
  std::string remainder;
  ifs.getLine(remainder);
  
  // if we reached end of file, this is not an error: we're done
//  if (ifs.eof()) return;
  
  throw TGenericException(T_SRCANDLINE,
                          "Madou1PceScriptReader::outputNextSymbol()",
                          "Line "
                            + TStringConversion::intToString(lineNum)
                            + ":\n  Couldn't match symbol at: '"
                            + remainder
                            + "'");
}
  
void Madou1PceScriptReader::flushActiveScript() {
  // write terminator
//  currentScriptBuffer.put(0x00);

  if (currentScriptBuffer.size() <= 0) return;

  int outputSize = currentScriptBuffer.size();
  
  ResultString result;
  currentScriptBuffer.seek(0);
  while (!currentScriptBuffer.eof()) {
    result.str += currentScriptBuffer.get();
  }
  
  result.srcOffset = currentScriptSrcOffset;
  result.srcPointer = currentScriptSrcPointer;
//  result.srcOpOffset = currentScriptSrcOpOffset;
//  result.index = currentScriptIndex;
//  result.name = currentScriptName;
  result.width = currentScriptWidth;
  result.indices = currentScriptIndices;
  
  dst[regionNum].push_back(result);
  
  // clear script buffer
  resetScriptBuffer();
}

void Madou1PceScriptReader::resetScriptBuffer() {
  currentScriptBuffer = TBufStream();
  currentScriptSrcOffset = -1;
  currentScriptSrcPointer = -1;
  currentScriptIndices.clear();
}
  
/*bool Madou1PceScriptReader::checkSymbol(BlackT::TStream& ifs, std::string& symbol) {
  if (symbol.size() > ifs.remaining()) return false;
  
  int startpos = ifs.tell();
  for (int i = 0; i < symbol.size(); i++) {
    if (symbol[i] != ifs.get()) {
      ifs.seek(startpos);
      return false;
    }
  }
  
  return true;
} */

void Madou1PceScriptReader::processDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("LOADTABLE") == 0) {
    processLoadTable(ifs);
  }
  else if (name.compare("STARTSCRIPT") == 0) {
    processStartMsg(ifs);
  }
  else if (name.compare("ENDSCRIPT") == 0) {
    processEndMsg(ifs);
  }
  else if (name.compare("INCBIN") == 0) {
    processIncBin(ifs);
  }
  else if (name.compare("BREAK") == 0) {
    processBreak(ifs);
  }
  else if (name.compare("STARTREGION") == 0) {
    processStartRegion(ifs);
  }
  else if (name.compare("ENDREGION") == 0) {
    processEndRegion(ifs);
  }
  else if (name.compare("SETLASTXPOS") == 0) {
    currentScriptWidth = TParse::matchInt(ifs);
  }
  else if (name.compare("SETINDEXLIST") == 0) {
    processSetIndexList(ifs);
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "Madou1PceScriptReader::processDirective()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ":\n  Unknown directive: "
                              + name);
  }
  
  TParse::matchChar(ifs, ')');
}

void Madou1PceScriptReader::processLoadTable(BlackT::TStream& ifs) {
  std::string tableName = TParse::matchString(ifs);
  TThingyTable table(tableName);
  loadThingy(table);
}

void Madou1PceScriptReader::processStartMsg(BlackT::TStream& ifs) {
//  if (TParse::checkInt(ifs)) {
    currentScriptSrcOffset = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptSrcPointer = TParse::matchInt(ifs);
//    TParse::matchChar(ifs, ',');
//    currentScriptSrcOpOffset = TParse::matchInt(ifs);
//    TParse::matchChar(ifs, ',');
//    currentScriptIndex = TParse::matchInt(ifs);
//  }
//  else if (TParse::checkString(ifs)) {
//    currentScriptName = TParse::matchString(ifs);
//  }
}

void Madou1PceScriptReader::processEndMsg(BlackT::TStream& ifs) {
  flushActiveScript();
}

void Madou1PceScriptReader::processIncBin(BlackT::TStream& ifs) {
  std::string filename = TParse::matchString(ifs);
  TBufStream src(1);
  src.open(filename.c_str());
  currentScriptBuffer.writeFrom(src, src.size());
}

void Madou1PceScriptReader::processBreak(BlackT::TStream& ifs) {
  breakTriggered = true;
}

void Madou1PceScriptReader::processStartRegion(BlackT::TStream& ifs) {
  flushActiveScript();
  regionNum = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  int pointerTableBase = TParse::matchInt(ifs);
}

void Madou1PceScriptReader::processEndRegion(BlackT::TStream& ifs) {
  TParse::matchInt(ifs);
//  TParse::matchInt(ifs);
////  flushActiveScript();
//  breakTriggered = true;
}

void Madou1PceScriptReader::processSetIndexList(BlackT::TStream& ifs) {
  currentScriptIndices.clear();
  while (TParse::checkInt(ifs)) {
    int next = TParse::matchInt(ifs);
    currentScriptIndices.push_back(next);
    if (!TParse::checkChar(ifs, ',')) break;
    TParse::matchChar(ifs, ',');
  }
}


}
