#ifndef TIFSTREAM_H
#define TIFSTREAM_H


#include "util/TStream.h"
#include <fstream>

namespace BlackT {


class TIfstream : public TStream {
public:
  TIfstream();
  TIfstream(const char* filename,
            std::ios_base::openmode mode = std::ios_base::in);
  virtual ~TIfstream();
  
  void open(const char* filename,
            std::ios_base::openmode mode = std::ios_base::in);
  void close();
  
  virtual char get();
  virtual void unget();
  virtual char peek();
  virtual void put(char c);
  virtual void read(char* dst, int size);
  virtual void write(const char* src, int size);
  virtual bool good() const;
  virtual bool bad() const;
  virtual bool fail() const;
  virtual bool eof() const;
  virtual void clear();
  virtual int tell();
  virtual void seek(int pos);
  virtual int size();
protected:
  std::ifstream ifs;
};


}


#endif
