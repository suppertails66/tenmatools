#include "tenma/TenmaMapIndex.h"
#include "tenma/TenmaMsgConsts.h"
#include "util/TEdcCalc.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


bool TenmaMapIndexEntry::matchesSector(BlackT::TStream& iso, int sectorNum) {
  sectorData.seek(0);
  iso.seek(sectorNum * TenmaMsgConsts::sectorSize);
  
  while (!sectorData.eof()) {
    if (sectorData.get() != iso.get()) return false;
  }
  
  return true;
}

TenmaMapIndex::TenmaMapIndex(BlackT::TStream& iso) {
/*  std::vector<int> mapSectorIndex = TenmaMsgConsts::getMapSectorIndex();
  for (std::vector<int>::iterator it = mapSectorIndex.begin();
       it != mapSectorIndex.end();
       ++it) {
    int mapBaseSector = *it;
//    for (int i = 0; i < TenmaMsgConsts::sectorsPerMapTextBlock; i++) {
//      iso.seek((mapBaseSector + i) * TenmaMsgConsts::sectorSize);
//      
//    }
    indexMap(iso, mapBaseSector);
  } */
}

int TenmaMapIndex::computeMapSize(BlackT::TStream& iso, int mapSectorNum) {
  for (int i = 1; i < TenmaMsgConsts::sectorsPerMapTextBlock; i++) {
//    std::cerr << i << std::endl;
    if (mapSectorMeetsDupeCriteria(iso, mapSectorNum, i)) return i;
  }
  
  return TenmaMsgConsts::sectorsPerMapTextBlock;
}

unsigned int TenmaMapIndex::getHash(BlackT::TStream& src, int size) {
  TBufStream ifs;
  ifs.writeFrom(src, size);
  TEdcCalc calc;
  return calc.calcEdc((unsigned char*)ifs.data().data(), size);
}

bool TenmaMapIndex::isoSectorsMatch(
    BlackT::TStream& iso, int firstSector, int secondSector) {
  TenmaMapIndexEntry temp;
  iso.seek(firstSector * TenmaMsgConsts::sectorSize);
  temp.sectorData.writeFrom(iso, TenmaMsgConsts::sectorSize);
  return temp.matchesSector(iso, secondSector);
}

void TenmaMapIndex::indexMap(BlackT::TStream& iso, int mapBaseSector) {
  for (int i = 0; i < TenmaMsgConsts::sectorsPerMapTextBlock; i++) {
    TenmaMapIndexEntry entry;
    entry.sectorNum = (mapBaseSector + i);
    
    iso.seek(entry.sectorNum * TenmaMsgConsts::sectorSize);
    entry.sectorData.writeFrom(iso, TenmaMsgConsts::sectorSize);
    entry.sectorData.seek(0);
    
    entry.hashVal = getHash(entry.sectorData, TenmaMsgConsts::sectorSize);
    hashToBuckets[i][entry.hashVal].push_back(entry);
  }
}

bool TenmaMapIndex::mapSectorMeetsDupeCriteria(
      BlackT::TStream& iso, int mapSectorNum, int sectorOffset) {
  // "dupe criteria" = the sectors at the specified offset match,
  // and at least one of the sectors preceding it in the map does not match
  
  iso.seek((mapSectorNum + sectorOffset) * TenmaMsgConsts::sectorSize);
  
  // check if the target sector matches any other sector
  unsigned int hashVal = getHash(iso, TenmaMsgConsts::sectorSize);
  TenmaMapIndexEntryCollection& targetBucketList
    = hashToBuckets.at(sectorOffset).at(hashVal);
  for (TenmaMapIndexEntryCollection::iterator it
         = targetBucketList.begin();
       it != targetBucketList.end();
       ++it) {
    TenmaMapIndexEntry& entry = *it;
    // does target sector match?
    if (entry.matchesSector(iso, mapSectorNum + sectorOffset)) {
      // does any sector below this one NOT match?
      bool nonMatchFound = false;
      for (int i = 0; i < sectorOffset; i++) {
        if (!isoSectorsMatch(
              iso,
              (mapSectorNum + sectorOffset) - i - 1,
              entry.sectorNum - i - 1)
            ) {
          nonMatchFound = true;
          break;
        }
      }
      
      if (nonMatchFound) return true;
    }
  }
  
  return false;
}


}
