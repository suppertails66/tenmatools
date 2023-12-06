#include "util/MiscMath.h"

namespace BlackT {


  int MiscMath::toBcd(int value) {
    int result = 0;
    int shift = 0;
    while (value > 0) {
      int base = (value % 10);
      result |= (base << shift);
      shift += 4;
      value /= 10;
    }
    
    return result;
  }
  
  int MiscMath::fromBcd(int value) {
    int result = 0;
    int mult = 1;
    while (value > 0) {
      int base = (value % 10);
      result += (base * mult);
      mult *= 10;
      value >>= 4;
    }
    
    return result;
  }
  
  int MiscMath::roundUpToBoundary(int value, int boundary) {
    int result = value / boundary;
    if ((value % boundary) != 0) ++result;
    return (result * boundary);
  }


}; 
