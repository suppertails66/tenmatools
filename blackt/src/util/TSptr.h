#ifndef TSPTR_H
#define TSPTR_H


#include "exception/TGenericException.h"
#include <cstdlib>
#include <iostream>

namespace BlackT {


template <class T>
class TSptr {
public:
  TSptr<T>()
    : t_(NULL) { }
  
  ~TSptr<T>() {
//    std::cerr << (long int)t_ << std::endl;
    delete t_;
  }
  
  TSptr<T>(T* t__) {
    t_ = t__;
  }
  
  TSptr<T>(const T& t__) {
    t_ = &t__;
  }
  
  T& operator*() {
    if (t_ == NULL) {
      throw TGenericException(T_SRCANDLINE,
                              "TSptr::operator*()",
                              "Dereferenced NULL pointer");
    }
  
    return *t_;
  }
  
  const T& operator*() const {
    if (t_ == NULL) {
      throw TGenericException(T_SRCANDLINE,
                              "TSptr::operator*() const",
                              "Dereferenced NULL pointer");
    }
  
    return *t_;
  }
  
  T* operator->() {
    if (t_ == NULL) {
      throw TGenericException(T_SRCANDLINE,
                              "TSptr::operator->()",
                              "Dereferenced NULL pointer");
    }
  
    return t_;
  }
  
  const T* operator->() const {
    if (t_ == NULL) {
      throw TGenericException(T_SRCANDLINE,
                              "TSptr::operator->() const",
                              "Dereferenced NULL pointer");
    }
  
    return t_;
  }
  
  void setPtr(T* t__) {
    t_ = t__;
  }
  
/*  TSptr<T>(const TSptr<T>& src)
    : t_(NULL) {
    if (src.t_ != NULL) {
      t_ = new T(*(src.t_));
    }
  }
  
  void operator=(const TSptr<T>& src) {
    delete t_;
    t_ = NULL;
    if (src.t_ != NULL) {
      t_ = new T(*(src.t_));
    }
  } */
  
protected:
  
  T* t_;
  
};

}


#endif
