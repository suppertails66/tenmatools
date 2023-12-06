#ifndef TGALOISFIELD256_H
#define TGALOISFIELD256_H


#include "util/TByte.h"
#include <initializer_list>
#include <vector>
#include <cstdlib>

namespace BlackT {


//typedef std::vector<TByte> TGaloisField256Polynomial;
  class TGaloisField256Polynomial {
  public:
    friend class TGaloisField256;
  
    TGaloisField256Polynomial();
    TGaloisField256Polynomial(int size__);
    TGaloisField256Polynomial(std::initializer_list<TByte> initList__);
    
    int size() const;
    int degree() const;
    TByte& operator[](int index);
    const TByte& operator[](int index) const;
  protected:
    std::vector<TByte> coeff;
  };

  // GF(2^8) representation
  // mostly based on freely-licensed code from https://research.swtch.com/field
  class TGaloisField256 {
  public:
    // primitivePoly: primitive polynomial (generator polynomial) for the field
    // primitiveElement: primitive element of the field
    TGaloisField256(int primitivePoly, int primitiveElem);
    
    TByte add(TByte a, TByte b) const;
    TByte exp(int a) const;
    TByte log(TByte a) const;
    TByte inv(TByte a) const;
    TByte mult(TByte a, TByte b) const;
    
    TGaloisField256Polynomial multPoly(
      const TGaloisField256Polynomial& a,
      const TGaloisField256Polynomial& b) const;
    
    TGaloisField256Polynomial calcReedSolomonCode(
      const TGaloisField256Polynomial& message,
      const TGaloisField256Polynomial& generator,
      int codewordLength,
      const TByte* generatorCoefficientLogArray = NULL) const;
  protected:
    
    // internal "slow" multiplication used to construct lookup tables
    int mult(int a, int b, int polynomial) const;
    
    TByte logTable[256];
    TByte expTable[510];
    
    int primitivePoly_;
    int primitiveElem_;
    
  };


}


#endif
