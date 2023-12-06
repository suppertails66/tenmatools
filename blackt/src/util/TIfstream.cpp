#include "util/TIfstream.h"

namespace BlackT {


TIfstream::TIfstream() { }

TIfstream::TIfstream(const char* filename,
          std::ios_base::openmode mode)
  : ifs(filename, mode) { }
          
TIfstream::~TIfstream() { }

void TIfstream::open(const char* filename,
          std::ios_base::openmode mode) {
  ifs.open(filename, mode);
}

void TIfstream::close() {
  ifs.close();
}

char TIfstream::get() {
  return ifs.get();
}

void TIfstream::unget() {
  ifs.unget();
}

char TIfstream::peek() {
  return ifs.peek();
}

void TIfstream::put(char c) {
  
}

void TIfstream::read(char* dst, int size) {
  ifs.read(dst, size);
}

void TIfstream::write(const char* src, int size) {
  
}

bool TIfstream::good() const {
  return ifs.good();
}

bool TIfstream::bad() const {
  return ifs.bad();
}

bool TIfstream::fail() const {
  return ifs.fail();
}

bool TIfstream::eof() const {
  return ifs.eof();
}

void TIfstream::clear() {
  ifs.clear();
}

int TIfstream::tell() {
  return ifs.tellg();
}

void TIfstream::seek(int pos) {
  ifs.seekg(pos);
}

int TIfstream::size() {
  int old = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  int sz = ifs.tellg();
  ifs.seekg(old);
  return sz;
}


}
