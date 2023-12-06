#ifndef TOPT_H
#define TOPT_H


namespace BlackT {


class TOpt {
public:
  
  static int findOpt(int argc, char* argv[], const char* str);

  static char* getOpt(int argc, char* argv[], const char* str);

  static bool hasFlag(int argc, char* argv[], const char* str);
  
  static void readNumericOpt(int argc, char* argv[], const char* str,
                             int* dst);
  
protected:
  
};


}


#endif
