#include "util/TFreeSpace.h"
#include <iostream>

namespace BlackT {


TFreeSpace::TFreeSpace()
  : boundarySize_(-1) { }

int TFreeSpace::claim(int size, int start, int end) {
//  int end = start + startsize;
//  std::cerr << end << std::endl;
  
  // find smallest possible space that will fit the specified data
  int smallest = -1;
  FreeSpaceMap::iterator bestIt = freeSpace_.end();
  for (FreeSpaceMap::iterator it = freeSpace_.begin();
       it != freeSpace_.end();
       ++it) {
    if ((it->second >= size)
        && ((smallest == -1)
            || (it->second < smallest))) {
      
      // if search range is limited, ignore segments that are not entirely
      // contained within search area
      if ((start != -1) && (end != -1)) {
        if ((it->first < start) || (it->first >= end)
             || ((it->first + size) > end)) continue;
      }
      
      smallest = it->second;
      bestIt = it;
    }
  }
  
  // not enough space available
  if (smallest == -1) {
    return -1;
  }

  // save old info (erasing will invalidate the iterator)
  int oldpos = bestIt->first;
  int oldsize = bestIt->second;
  int newpos = oldpos + size;
  int newsize = oldsize - size;
  
  freeSpace_.erase(bestIt);
  
  // remap remaining free space in block (if nonzero)
  if (newsize > 0) {
    freeSpace_[newpos] = newsize;
  }
  
  return oldpos;
}

void TFreeSpace::free(int offset, int size) {
  if (size == 0) return;
  // completely ignore requests for existing blocks
  // (the intention is to ignore duplicates)
  if (freeSpace_.find(offset) != freeSpace_.end()) return;
  
  // Check if new block is adjacent to an existing one
  FreeSpaceMap::iterator findIt = findAdjacentBlock(offset, size,
    freeSpace_.begin());
  while (findIt != freeSpace_.end()) {
    // combine blocks if boundary allows
    int oldpos = findIt->first;
    int oldsize = findIt->second;
    
    int newOffset = offset;
    int newSize = oldsize + size;
    
    // newOffset must be lower number
    if (oldpos < offset) {
      newOffset = oldpos;
      oldpos = offset;
    }
    
    int newEnd = offset + newSize;
    
    if (boundarySize_ != -1) {
      
      // check if block combine would cross bank boundary
      int bank0 = newOffset / boundarySize_;
      int bank1 = (newEnd - 1) / boundarySize_;
      
      if (bank0 != bank1) goto noCombine;
      
    }
    
    freeSpace_.erase(findIt);
    freeSpace_[newOffset] = newSize;
    
    // defragment the space
    defragment();
    return;
    
    noCombine:
      ++findIt;
      findIt = findAdjacentBlock(offset, size, findIt);
  }
  
  // Does ending address overlap?
/*  FreeSpaceMap::iterator findIt = findBlockStart(offset + size);
  if (findIt != freeSpace.end()) {
    int oldpos = findIt->first;
    int oldsize = findIt->second;
    freeSpace.erase(findIt);
    freeSpace[offset] = size + oldsize;
    
    defragment();
    return;
  } */
  
  freeSpace_[offset] = size;
  defragment();
}
  
void TFreeSpace::setBoundarySize(int boundarySize__) {
  boundarySize_ = boundarySize__;
}

void TFreeSpace::defragment() {
  if (freeSpace_.size() <= 1) return;

  FreeSpaceMap::iterator it = freeSpace_.begin();
  FreeSpaceMap::iterator prevIt = it;
  ++it;
  
  while (it != freeSpace_.end()) {
    ++it;
    FreeSpaceMap::iterator nextIt = it;
    --it;
    
    if (prevIt->first + prevIt->second == it->first) {
      
      if (boundarySize_ != -1) {
        
        // check if block combine would cross bank boundary
        int bank0 = prevIt->first / boundarySize_;
        int bank1 = (prevIt->first + prevIt->second + it->second - 1)
          / boundarySize_;
        
        if (bank0 != bank1) goto noCombine;
      }
    
      freeSpace_.erase(it);
      freeSpace_[prevIt->first] = prevIt->second + it->second;
    }
    noCombine:
    
    it = nextIt;
  }
  
}

TFreeSpace::FreeSpaceMap::iterator TFreeSpace::findBlockStart(int offset) {
  for (FreeSpaceMap::iterator it = freeSpace_.begin();
       it != freeSpace_.end();
       ++it) {
    if (offset < it->first) continue;
    if (offset < (it->first + it->second)) return it;
  }
  
  // No block contains specified address
  return freeSpace_.end();
}

TFreeSpace::FreeSpaceMap::iterator TFreeSpace
    ::findAdjacentBlock(int offset, int size,
        FreeSpaceMap::iterator startPos) {
  for (FreeSpaceMap::iterator it = startPos;
       it != freeSpace_.end();
       ++it) {
    if ((offset + size == it->first)) return it;
    if (it->first + it->second == offset) return it;
  }
  
  // No block contains specified address
  return freeSpace_.end();
}


}
