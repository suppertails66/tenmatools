#ifndef TOFSTREAM_H
#define TOFSTREAM_H


#include "util/TStream.h"
#include <fstream>

namespace BlackT {


class TOfstream : public TStream {
public:
  TOfstream();
  TOfstream(const char* filename,
            std::ios_base::openmode mode = std::ios_base::out);
  virtual ~TOfstream();
  
  void open(const char* filename,
            std::ios_base::openmode mode = std::ios_base::out);
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
  std::ofstream ofs;
};


}


#endif
