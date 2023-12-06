#include "util/TOfstream.h"

namespace BlackT {


TOfstream::TOfstream() { }

TOfstream::TOfstream(const char* filename,
          std::ios_base::openmode mode)
  : ofs(filename, mode) { }
          
TOfstream::~TOfstream() { }

void TOfstream::open(const char* filename,
          std::ios_base::openmode mode) {
  ofs.open(filename, mode);
}

void TOfstream::close() {
  ofs.close();
}

char TOfstream::get() {
  put(0);
  return 0;
}

void TOfstream::unget() {
  
}

char TOfstream::peek() {
  return 0;
}

void TOfstream::put(char c) {
  ofs.put(c);
}

void TOfstream::read(char* dst, int size) {
  
}

void TOfstream::write(const char* src, int size) {
  ofs.write(src, size);
}

bool TOfstream::good() const {
  return ofs.good();
}

bool TOfstream::bad() const {
  return ofs.bad();
}

bool TOfstream::fail() const {
  return ofs.fail();
}

bool TOfstream::eof() const {
  return ofs.eof();
}

void TOfstream::clear() {
  ofs.clear();
}

int TOfstream::tell() {
  return ofs.tellp();
}

void TOfstream::seek(int pos) {
  ofs.seekp(pos);
}

int TOfstream::size() {
  int old = ofs.tellp();
  ofs.seekp(0, std::ios_base::end);
  int sz = ofs.tellp();
  ofs.seekp(old);
  return sz;
}


}
