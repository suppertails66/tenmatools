#ifndef TENMAMAPINDEX_H
#define TENMAMAPINDEX_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include <string>
#include <vector>
#include <list>
#include <map>

namespace Pce {


class TenmaMapIndexEntry {
public:
  
  BlackT::TBufStream sectorData;
  int sectorNum;
  unsigned int hashVal;
  
  bool matchesSector(BlackT::TStream& iso, int sectorNum);
  
protected:
  
};

typedef std::list<TenmaMapIndexEntry> TenmaMapIndexEntryCollection;

class TenmaMapIndex {
public:
  
  TenmaMapIndex(BlackT::TStream& iso);
  
  int computeMapSize(BlackT::TStream& iso, int mapSectorNum);
  
protected:
  // map of hash values to bucket list containing sectors with that hash
  typedef std::map<unsigned int, TenmaMapIndexEntryCollection>
    SectorHashToBucketsMap;
  // map of sector offsets (0-3) to hash->bucket map
  typedef std::map<int, SectorHashToBucketsMap> SectorIndexToHashesMap;
  
  SectorIndexToHashesMap hashToBuckets;
  
  static unsigned int getHash(BlackT::TStream& src, int size);
  static bool isoSectorsMatch(
      BlackT::TStream& iso, int firstSector, int secondSector);
  void indexMap(BlackT::TStream& iso, int mapBaseSector);
  bool mapSectorMeetsDupeCriteria(BlackT::TStream& iso, int mapSectorNum,
                                 int sectorOffset);
  
};


}


#endif
