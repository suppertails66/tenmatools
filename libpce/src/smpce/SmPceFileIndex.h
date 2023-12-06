#ifndef SMPCEFILEINDEX_H
#define SMPCEFILEINDEX_H


#include "util/TStream.h"
#include "util/TByte.h"
#include <string>
#include <vector>
#include <iostream>

namespace Pce {


struct SmPceFileIndexEntry {
  std::string hash;
  int recNum;
  BlackT::TByte recByte1;
  BlackT::TByte recByte2;
  
  int getBytePos() const;
  int getByteSize() const;
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
};

class SmPceFileIndex {
public:
  
  SmPceFileIndex();
  
  void read(BlackT::TStream& ifs, unsigned int numEntries);
  
  SmPceFileIndexEntry* findEntry(const std::string& name);
  SmPceFileIndexEntry* findEntryByHash(const std::string& hash);
  
  typedef std::vector<SmPceFileIndexEntry> SmPceFileIndexEntryCollection;
  SmPceFileIndexEntryCollection entries;
  
protected:
  
};


}


#endif
