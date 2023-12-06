#include "util/TStream.h"
#include "util/TSerialize.h"
#include <cctype>

namespace BlackT { 


int TStream::remaining() {
  return (size() - tell());
}

void TStream::seekoff(int offset) {
  seek(tell() + offset);
}

int TStream::readInt(
            int sz,
            EndiannessTypes::EndiannessType end,
            SignednessTypes::SignednessType sign) {
  return TSerialize::readInt(*this, sz, end, sign);
}

void TStream::writeInt(
            int value,
            int sz,
            EndiannessTypes::EndiannessType end,
            SignednessTypes::SignednessType sign) {
  TSerialize::writeInt(*this, value, sz, end, sign);
}

void TStream::readCstr(char* dst, int limit) {
  char c;
  int pos = 0;
  while ((c = get())) {
    if ((limit != -1) && (pos >= limit)) break;
    dst[pos++] = c;
  }
  
  // write terminator
  dst[pos++] = c;
}

void TStream::writeCstr(const char* src, int limit) {
  char c;
  int pos = 0;
  while ((c = src[pos++])) {
    if ((limit != -1) && (pos >= limit)) break;
    put(c);
  }
  
  // write terminator
  put(c);
}

void TStream::readCstrString(std::string& dst, int limit) {
  char c;
  int pos = 0;
  while ((c = get())) {
    if ((limit != -1) && (pos >= limit)) break;
    dst += c;
  }
}

void TStream::readRev(char* dst, int size) {
  read(dst, size);
  char* first = dst;
  char* last = dst + size - 1;
  while (first < last) {
    char temp = *last;
    *last = *first;
    *first = temp;
    ++first;
    --last;
  }
}

void TStream::writeRev(const char* src, int size) {
  // the lazy way
  const char* srcend = src + size - 1;
  while (srcend >= src) {
    write(srcend--, 1);
  }
}

void TStream::alignToBoundary(int byteBoundary) {
  int amt = byteBoundary - (size() % byteBoundary);
  if (amt != byteBoundary) {
    for (int i = 0; i < amt; i++) {
      get();
    }
  }
}

void TStream::alignToReadBoundary(int byteBoundary) {
  int amt = byteBoundary - (tell() % byteBoundary);
  if (amt != byteBoundary) {
    seekoff(amt);
  }
}

void TStream::alignToWriteBoundary(int byteBoundary) {
  alignToBoundary(byteBoundary);
}

void TStream::padToSize(int sz, char fillChar) {
  while (size() < sz) {
    put(fillChar);
  }
}

bool TStream::nextIsEof() {
  get();
  bool result = eof();
  unget();
  return result;
}

void TStream::write(const std::string& str) {
//  write(str.c_str(), str.size());
  writeString(str);
}

void TStream::writeString(const std::string& str) {
  for (unsigned int i = 0; i < str.size(); i++) put(str[i]);
}

void TStream::getLine(std::string& dst) {
  while (!eof() && (peek() != '\n')) {
    char next = get();
    if (next != '\r') {
      dst += next;
    }
  }
  
  if (!eof()) get();
}

void TStream::skipSpace() {
  while (!eof() && isspace(peek())) get();
}

TStream::TStream() { }
TStream::~TStream() { }

int TStream::reads8le() {
  return readInt(1, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::reads16le() {
  return readInt(2, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::reads32le() {
  return readInt(4, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::reads64le() {
  return readInt(8, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::readu8le() {
  return readInt(1, EndiannessTypes::little, SignednessTypes::nosign);
}

int TStream::readu16le() {
  return readInt(2, EndiannessTypes::little, SignednessTypes::nosign);
}

int TStream::readu32le() {
  return readInt(4, EndiannessTypes::little, SignednessTypes::nosign);
}

int TStream::readu64le() {
  return readInt(8, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writes8le(int value) {
  writeInt(value, 1, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writes16le(int value) {
  writeInt(value, 2, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writes32le(int value) {
  writeInt(value, 4, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writes64le(int value) {
  writeInt(value, 8, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writeu8le(int value) {
  writeInt(value, 1, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writeu16le(int value) {
  writeInt(value, 2, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writeu32le(int value) {
  writeInt(value, 4, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writeu64le(int value) {
  writeInt(value, 8, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writes8be(int value) {
  writeInt(value, 1, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writes16be(int value) {
  writeInt(value, 2, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writes32be(int value) {
  writeInt(value, 4, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writes64be(int value) {
  writeInt(value, 8, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writeu8be(int value) {
  writeInt(value, 1, EndiannessTypes::big, SignednessTypes::nosign);
}

void TStream::writeu16be(int value) {
  writeInt(value, 2, EndiannessTypes::big, SignednessTypes::nosign);
}

void TStream::writeu32be(int value) {
  writeInt(value, 4, EndiannessTypes::big, SignednessTypes::nosign);
}

void TStream::writeu64be(int value) {
  writeInt(value, 8, EndiannessTypes::big, SignednessTypes::nosign);
}

void TStream::writes16lebe(int value) {
  writes16le(value);
  writes16be(value);
}

void TStream::writes32lebe(int value) {
  writes32le(value);
  writes32be(value);
}

void TStream::writes64lebe(int value) {
  writes64le(value);
  writes64be(value);
}

void TStream::writeu16lebe(int value) {
  writeu16le(value);
  writeu16be(value);
}

void TStream::writeu32lebe(int value) {
  writeu32le(value);
  writeu32be(value);
}

void TStream::writeu64lebe(int value) {
  writeu64le(value);
  writeu64be(value);
}

int TStream::reads8be() {
  return readInt(1, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::reads16be() {
  return readInt(2, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::reads32be() {
  return readInt(4, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::reads64be() {
  return readInt(8, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::readu8be() {
  return readInt(1, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::readu16be() {
  return readInt(2, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::readu32be() {
  return readInt(4, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::readu64be() {
  return readInt(8, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::reads16lebe() {
  seekoff(2);
  return reads16be();
}

int TStream::reads32lebe() {
  seekoff(4);
  return reads32be();
}

int TStream::reads64lebe() {
  seekoff(8);
  return reads64be();
}

int TStream::readu16lebe() {
  seekoff(2);
  return readu16be();
}

int TStream::readu32lebe() {
  seekoff(4);
  return readu32be();
}

int TStream::readu64lebe() {
  seekoff(8);
  return readu64be();
}

int TStream::readu8() {
  return readu8be();
}

int TStream::reads8() {
  return reads8be();
}

void TStream::writeu8(int value) {
  writeu8be(value);
}

void TStream::writes8(int value) {
  writes8be(value);
}


}
