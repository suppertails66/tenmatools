#ifndef TENMAMSGDISMEXCEPTION_H
#define TENMAMSGDISMEXCEPTION_H


#include "exception/TException.h"
#include <exception>
#include <string>

namespace Pce {


class TenmaMsgDismException : public BlackT::TException {
public:
  
  TenmaMsgDismException();
  
  TenmaMsgDismException(const char* nameOfSourceFile__,
                              int lineNum__,
                              const std::string& source__);
  
  TenmaMsgDismException(const char* nameOfSourceFile__,
                              int lineNum__,
                              const std::string& source__,
                              const std::string& problem__);
  
  std::string problem() const;
  
protected:
  
  std::string problem_;
  
};


}


#endif
