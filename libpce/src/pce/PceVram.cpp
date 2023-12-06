#include "pce/PceVram.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Pce {


PceVram::PceVram()
  : data_(dataSize) {
  // zero-init
  for (int i = 0; i < dataSize; i++) data_[i] = 0;
}

PcePattern PceVram::getPattern(int patternNum) const {
  if (patternNum >= numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "PceVram::getPattern()",
                            std::string("Out-of-range pattern number: ")
                              + TStringConversion::intToString(patternNum));
  }
  
  TBufStream ifs;
  ifs.write((char*)(data_.data() + (patternNum * PcePattern::size)),
            PcePattern::size);
  
  ifs.seek(0);
  PcePattern pattern;
  pattern.read(ifs);
  
  return pattern;
}

PceSpritePattern PceVram::getSpritePattern(int patternNum) const {
  if (patternNum >= numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "PceVram::getSpritePattern()",
                            std::string("Out-of-range pattern number: ")
                              + TStringConversion::intToString(patternNum));
  }
  
  TBufStream ifs;
  ifs.write((char*)(data_.data() + (patternNum * PceSpritePattern::size)),
            PceSpritePattern::size);
  
  ifs.seek(0);
  PceSpritePattern pattern;
  pattern.read(ifs);
  
  return pattern;
}

void PceVram::setPattern(int patternNum, const PcePattern& src) {
  if (patternNum >= numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "PceVram::setPattern()",
                            std::string("Out-of-range pattern number: ")
                              + TStringConversion::intToString(patternNum));
  }

  TBufStream temp;
  src.write(temp);
  temp.seek(0);
  temp.read((char*)(data_.data() + (patternNum * PcePattern::size)),
            PcePattern::size);
}

void PceVram::setSpritePattern(int patternNum, const PceSpritePattern& src) {
  if (patternNum >= numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "PceVram::setSpritePattern()",
                            std::string("Out-of-range pattern number: ")
                              + TStringConversion::intToString(patternNum));
  }

  TBufStream temp;
  src.write(temp);
  temp.seek(0);
  temp.read((char*)(data_.data() + (patternNum * PceSpritePattern::size)),
            PceSpritePattern::size);
}

void PceVram::readPatterns(BlackT::TStream& src, int patternNum, int count) {
  if (count == -1) {
    count = src.remaining() / PcePattern::size;
  }
  
  if (patternNum + count > numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "PceVram::readPatterns()",
                            std::string("Loading ")
                              + TStringConversion::intToString(count)
                              + " patterns to "
                              + TStringConversion::intToString(patternNum)
                              + " would overflow VRAM");
  }
  
  for (int i = 0; i < count; i++) {
    PcePattern pattern;
    pattern.read(src);
    setPattern(patternNum + i, pattern);
  }
}

void PceVram::readSpritePatterns(
    BlackT::TStream& src, int patternNum, int count) {
  if (count == -1) {
    count = src.remaining() / PcePattern::size;
  }
  
  if (patternNum + count > numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "PceVram::readSpritePatterns()",
                            std::string("Loading ")
                              + TStringConversion::intToString(count)
                              + " patterns to "
                              + TStringConversion::intToString(patternNum)
                              + " would overflow VRAM");
  }
  
  for (int i = 0; i < count; i++) {
    PceSpritePattern pattern;
    pattern.read(src);
    setSpritePattern(patternNum + (i * 4), pattern);
  }
}




}
