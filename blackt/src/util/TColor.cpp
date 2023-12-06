#include "util/TColor.h"

namespace BlackT {


TColor::TColor()
 : a_(0),
   r_(0),
   g_(0),
   b_(0) { };

TColor::TColor(TByte r__,
      TByte g__,
      TByte b__,
      TByte a__)
  : a_(a__),
    r_(r__),
    g_(g__),
    b_(b__) { };

TColor::TColor(const TByte* src)
  : a_(src[3]),
    r_(src[2]),
    g_(src[1]),
    b_(src[0]) { };
    
bool TColor::operator==(const BlackT::TColor& other) const {
  if ((r_ == other.r_)
      && (g_ == other.g_)
      && (b_ == other.b_)
      && (a_ == other.a_)) return true;
  return false;
}

bool TColor::operator!=(const BlackT::TColor& other) const {
  return !(*this == other);
}
        
TByte TColor::a() const {
  return a_;
}

TByte TColor::r() const {
  return r_;
}

TByte TColor::g() const {
  return g_;
}

TByte TColor::b() const {
  return b_;
}

void TColor::setA(TByte a__) {
  a_ = a__;
}

void TColor::setR(TByte r__) {
  r_ = r__;
}

void TColor::setG(TByte g__) {
  g_ = g__;
}

void TColor::setB(TByte b__) {
  b_ = b__;
}
  
long int TColor::asInt() const {
  return ((long int)r_
                  | ((long int)g_ << 8)
                  | ((long int)b_ << 16)
                  | ((long int)a_ << 24));
}

void TColor::fromInt(long int value) {
  r_ = (value & 0x000000FF);
  g_ = (value & 0x0000FF00) >> 8;
  b_ = (value & 0x00FF0000) >> 16;
  a_ = (value & 0xFF000000) >> 24;
}
  
bool TColor::operator<(const TColor& other) const {
  return (asInt() < other.asInt());
}


};
