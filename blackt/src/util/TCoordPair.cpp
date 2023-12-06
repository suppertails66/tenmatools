#include "util/TCoordPair.h"

namespace BlackT {


TCoordPair::TCoordPair()
  : x_(0),
    y_(0) { }

TCoordPair::TCoordPair(int x__, int y__)
  : x_(x__),
    y_(y__) { }

int TCoordPair::x() const {
  return x_;
}

void TCoordPair::setX(int x__) {
  x_ = x__;
}

int TCoordPair::y() const {
  return y_;
}

void TCoordPair::setY(int y__) {
  y_ = y__;
}


} 
