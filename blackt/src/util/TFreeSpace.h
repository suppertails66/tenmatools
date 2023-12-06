#ifndef TFREESPACE_H
#define TFREESPACE_H


#include "util/TArray.h"
#include <map>

namespace BlackT {


/**
 * Class that tracks noncontiguous areas within a memory space and allows
 * them to be "claimed" and "freed". Overlapping areas are resolved after
 * each operation.
 */
class TFreeSpace {
public:
  typedef std::map<int, int> FreeSpaceMap;
  
  TFreeSpace();
  
  /**
   * Claim an area of free space of the given size. The claimed area is
   * removed from the memory model.
   * 
   * @param size Number of bytes of area to claim.
   * @param start Minimum starting address of search.
   * @param end Maximum ending address of search.
   *
   * @return The address of the claimed space, or -1 if a contiguous block
   * of the specified size does not exist.
   */
  int claim(int size, int start = -1, int end = -1);
  
  /**
   * Free an area of space of the given size. The freed area is added
   * to the memory model.
   * 
   * @param offset Offset of the block to free.
   * @param size Number of bytes of area to free.
   */
  void free(int offset, int size);
  
  void setBoundarySize(int boundarySize__);
  
  FreeSpaceMap freeSpace_;
protected:
  
  int boundarySize_;
  
  /**
   * Defragment the memory space, eliminating overlapping areas.
   */
  void defragment();
  
  /**
   * Find a free block containing the specified offset.
   *
   * @return A FreeSpaceMap::iterator to the containing block, or a iterator
   * to freeSpace_.end() if the specified address is not contained in any
   * block.
   */
  FreeSpaceMap::iterator findBlockStart(int offset);
  
  FreeSpaceMap::iterator findAdjacentBlock(int offset, int size,
        FreeSpaceMap::iterator startPos);
  
};


}


#endif
