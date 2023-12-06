#include "util/TGaloisField256.h"

namespace BlackT {


  TGaloisField256Polynomial::TGaloisField256Polynomial()
    : coeff(0) {
    
  }
  
  TGaloisField256Polynomial::TGaloisField256Polynomial(int size__)
    : coeff(size__) {
    
  }
  
  TGaloisField256Polynomial::TGaloisField256Polynomial(
      std::initializer_list<TByte> initList__)
    : coeff(initList__.size()) {
    int pos = 0;
    for (std::initializer_list<TByte>::iterator it = initList__.begin();
         it != initList__.end();
         ++it) {
//      coeff[coeff.size() - 1 - pos] = *it;
      coeff[pos] = *it;
      ++pos;
    }
  }
  
  int TGaloisField256Polynomial::size() const {
    return coeff.size();
  }
  
  int TGaloisField256Polynomial::degree() const {
    for (int i = coeff.size() - 1; i >= 0; i--) {
      if (coeff[i] != 0) return i;
    }
    
    return 0;
  }
  
  TByte& TGaloisField256Polynomial::operator[](int index) {
//    return coeff.at(coeff.size() - 1 - index);
    return coeff.at(index);
  } 
  
  const TByte& TGaloisField256Polynomial::operator[](int index) const {
//    return coeff.at(coeff.size() - 1 - index);
    return coeff.at(index);
  }
    
  TGaloisField256::TGaloisField256(int primitivePoly, int primitiveElem)
    : primitivePoly_(primitivePoly),
      primitiveElem_(primitiveElem) {
    int next = 1;
    for (int i = 0; i < 255; i++) {
      expTable[i] = next;
      expTable[i + 255] = next;
      logTable[next] = i;
      
      // progressively produce elem^0, elem^1, elem^2, ..., elem^254
      next = mult(next, primitiveElem, primitivePoly);
    }
    logTable[0] = 255;
  }
  
  TByte TGaloisField256::add(TByte a, TByte b) const {
    return a ^ b;
  }
  
  TByte TGaloisField256::exp(int a) const {
    if (a < 0) return 0;
    return expTable[a % 255];
  }
  
  TByte TGaloisField256::log(TByte a) const {
    if (a == 0) return -1;
    return logTable[a];
  }
  
  TByte TGaloisField256::mult(TByte a, TByte b) const {
    if ((a == 0) || (b == 0)) return 0;
    return expTable[(int)logTable[a] + (int)logTable[b]];
/*    int result = 0;
    int a2 = a;
    int b2 = b;
    while (a2 > 0) {
      if ((a2 & 0x01) != 0) result ^= b2;
      a2 >>= 1;
      b2 <<= 1;
      // reduce modulo the polynomial
      if ((b2 & 0x100) != 0) b2 ^= primitivePoly_;
    }
    return result; */
  }
  
  TByte TGaloisField256::inv(TByte a) const {
    if (a == 0) return 0;
    return expTable[255 - logTable[a]];
  }

  int TGaloisField256::mult(int a, int b, int polynomial) const {
    int result = 0;
    while (a > 0) {
      if ((a & 0x01) != 0) result ^= b;
      a >>= 1;
      b <<= 1;
      // reduce modulo the polynomial
      if ((b & 0x100) != 0) b ^= polynomial;
    }
    return result;
  }
    
  TGaloisField256Polynomial TGaloisField256::multPoly(
      const TGaloisField256Polynomial& a,
      const TGaloisField256Polynomial& b) const {
    // create result polynomial of appropriate size to contain highest
    // possible degree that may result
    int resultSize = a.size() + b.size() - 1;
    TGaloisField256Polynomial result
      = TGaloisField256Polynomial(resultSize);
    // initialize coefficients to zero
    for (int i = 0; i < result.size(); i++) result[i] = 0;
    
    for (int j = 0; j < b.size(); j++) {
      for (int i = 0; i < a.size(); i++) {
        int degree = i + j;
        TByte amount = mult(a[i], b[j]);
        result[degree] = add(result[degree], amount);
      }
    }
    
    return result;
  }
  
/*  TGaloisField256Polynomial TGaloisField256::calcReedSolomonCode(
      const TGaloisField256Polynomial& message,
      const TGaloisField256Polynomial& generator,
      int codewordLength) const {
    int codeSize = message.size() - codewordLength;
    TGaloisField256Polynomial result
      = TGaloisField256Polynomial(codewordLength);
    // multiply message by x^codeSize and store in high coefficients of result
    for (int i = 0; i < message.size(); i++)
      result[i] = message[i];
    // zero low coefficients
    for (int i = codewordLength; i < result.size(); i++)
      result[i] = 0;
    
    for (int i = 0; i < message.size(); i++) {
      TByte divVal = mult(result[i], inv(generator[0]));
      for (int j = 0; j < generator.size(); j++) {
        result[i + j] = add(result[i + j], mult(divVal, generator[j]));
      }
    }
    
    return result;
  } */
  
  TGaloisField256Polynomial TGaloisField256::calcReedSolomonCode(
      const TGaloisField256Polynomial& message,
      const TGaloisField256Polynomial& generator,
      int codewordLength,
      const TByte* generatorCoefficientLogArray) const {
    int codeSize = message.size() - codewordLength;
    TGaloisField256Polynomial result
      = TGaloisField256Polynomial(codewordLength);
    
    // multiply message by x^codeSize and store in high coefficients of result
    for (int i = 0; i < message.size(); i++)
      result[i] = message[i];
    // zero low coefficients
    for (int i = codewordLength; i < result.size(); i++)
      result[i] = 0;
    
    const TByte* genLogs = generatorCoefficientLogArray;
    
    // compute coefficient log lookup table if not provided
    TByte genLogsArray[256];
    if (generatorCoefficientLogArray == NULL) {
      for (int i = 0; i < generator.size(); i++) {
        genLogsArray[i] = (generator[i] == 0) ? 255 : logTable[generator[i]];
      }
      genLogs = genLogsArray;
    }
    
    for (int i = 0; i < message.size(); i++) {
      if (result[i] == 0) continue;
      
      TByte divValLog = logTable[result[i]];
      for (int j = 0; j < generator.size(); j++) {
        if (genLogs[j] != 255)
          result[i + j] ^= expTable[(int)divValLog + (int)genLogs[j]];
      }
    }
    
    return result;
  }


} 
