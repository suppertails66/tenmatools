#include "util/TOpt.h"
#include "util/TStringConversion.h"
#include <string>
#include <cstring>
#include <cstdlib>

namespace BlackT {


int TOpt::findOpt(int argc, char* argv[], const char* str) {
  for (int i = 0; i < argc - 1; i++) {
    if (strcmp(argv[i], str) == 0) {
      return i;
    }
  }
  
  return -1;
}

char* TOpt::getOpt(int argc, char* argv[], const char* str) {
  int result = findOpt(argc, argv, str);
  if (result == -1) return NULL;
  return argv[result + 1];
}

bool TOpt::hasFlag(int argc, char* argv[], const char* str) {
  int result = findOpt(argc + 1, argv, str);
  if (result == -1) return false;
  return true;
}
  
void TOpt::readNumericOpt(int argc, char* argv[], const char* str,
                          int* dst) {
  char* optP = getOpt(argc, argv, str);
  if (optP != NULL) {
    *dst = TStringConversion::stringToInt(std::string(optP));
  }
}


} 
