#include "smpce/SmPceFileIndex.h"
#include "smpce/SmPceScriptOps.h"
#include "smpce/SmPceNameHasher.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


int SmPceFileIndexEntry::getBytePos() const {
  return (recNum * 0x800);
}

int SmPceFileIndexEntry::getByteSize() const {
  // recByte1 and recByte2 are just the little-endian bytesize of the file;
  // I'd already replicated the original code before I realized this

  // divide high byte of bytesize by 8
/*  int numRecords = (recByte2 >> 3);
  
  // if there is partial data for an additional sector, factor it in
  if (((recByte2 & 0x07) == 0) && (recByte1 == 0)) {
    
  }
  else {
    ++numRecords;
  }
  
  return (numRecords * 0x800); */
  
  int result = (unsigned char)recByte2;
  result <<= 8;
  result |= (unsigned char)recByte1;
  
  return result;
}

void SmPceFileIndexEntry::read(BlackT::TStream& ifs) {
  int intHash = ifs.readInt(3, EndiannessTypes::big, SignednessTypes::nosign);
  std::string rawHash = TStringConversion::intToString(intHash,
    TStringConversion::baseHex).substr(2, std::string::npos);
  while (rawHash.size() < 6) rawHash = std::string("0") + rawHash;
  hash = rawHash;
  
  recNum = ifs.readu16be();
  recByte1 = ifs.get();
  recByte2 = ifs.get();
}

void SmPceFileIndexEntry::write(BlackT::TStream& ofs) const {
  int intHash = TStringConversion::stringToInt(std::string("0x") + hash);
  ofs.writeInt(intHash, 3, EndiannessTypes::big, SignednessTypes::nosign);
  ofs.writeu16be(recNum);
  ofs.put(recByte1);
  ofs.put(recByte2);
}

SmPceFileIndex::SmPceFileIndex() {
  
}

void SmPceFileIndex::read(BlackT::TStream& ifs, unsigned int numEntries) {
  entries.resize(numEntries);
  for (unsigned int i = 0; i < numEntries; i++) {
    entries[i].read(ifs);
  }
}

SmPceFileIndexEntry* SmPceFileIndex::findEntry(const std::string& name) {
  std::string hash = SmPceNameHasher::hash(name);
  
  return findEntryByHash(hash);
}

SmPceFileIndexEntry* SmPceFileIndex::findEntryByHash(const std::string& hash) {
  for (unsigned int i = 0; i < entries.size(); i++) {
    if (entries[i].hash.compare(hash) == 0) {
      return &(entries[i]);
    }
  }
  
  return NULL;
}


}
