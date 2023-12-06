#include "tenma/TenmaMsgDismException.h"

using namespace BlackT;

namespace Pce {


TenmaMsgDismException::TenmaMsgDismException()
  : TException() { };

TenmaMsgDismException::TenmaMsgDismException(
    const char* nameOfSourceFile__,
    int lineNum__,
    const std::string& source__)
  : TException(nameOfSourceFile__,
               lineNum__,
               source__) { };

TenmaMsgDismException::TenmaMsgDismException(
    const char* nameOfSourceFile__,
    int lineNum__,
    const std::string& source__,
    const std::string& problem__)
  : TException(nameOfSourceFile__,
               lineNum__,
               source__),
    problem_(problem__) { };

std::string TenmaMsgDismException::problem() const {
  return problem_;
}


} 
