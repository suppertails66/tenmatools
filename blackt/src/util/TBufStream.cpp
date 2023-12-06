#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include <cstring>
#include <iostream>

namespace BlackT {


TBufStream::TBufStream(int sz)
  : pos_(0),
    endPos_(0) {
  data_.resize(sz);
  std::memset(data_.data(), 0, sz);
}

TBufStream::~TBufStream() { }
  
void TBufStream::open(const char* filename) {
  TIfstream ifs(filename, std::ios_base::binary);
  data_.resize(ifs.size());
  ifs.read(data_.data(), data_.size());
  pos_ = data_.size();
  
  updateEndPos();
  
  pos_ = 0;
}

void TBufStream::save(const char* filename) {
  TOfstream ofs(filename, std::ios_base::binary);
  ofs.write(data_.data(), endPos_);
}

char TBufStream::get() {
  return data_[pos_++];
}

void TBufStream::unget() {
  --pos_;
}

char TBufStream::peek() {
  return data_[pos_];
}

void TBufStream::put(char c) {
  recapIfNeeded(endPos_ + 1);
  
  data_[pos_++] = c;
  
  updateEndPos();
}

void TBufStream::read(char* dst, int size) {
  std::memcpy(dst, data_.data() + pos_, size);
  pos_ += size;
}

void TBufStream::write(const char* src, int size) {
  recapIfNeeded(endPos_ + size);
  
  std::memcpy(data_.data() + pos_, src, size);
  pos_ += size;
  
  updateEndPos();
}

bool TBufStream::good() const {
  return (bad() || fail() || eof());
}

bool TBufStream::bad() const {
  return false;
}

bool TBufStream::fail() const {
  return eof();
}

bool TBufStream::eof() const {
  return (pos_ >= endPos_);
}

void TBufStream::clear() {
  
}

int TBufStream::tell() {
  return pos_;
}

void TBufStream::seek(int pos) {
  pos_ = pos;
  
  updateEndPos();
}

int TBufStream::size() {
  return endPos_;
}

void TBufStream::alignToBoundary(int byteBoundary) {
  int amt = byteBoundary - (pos_ % byteBoundary);
  if (amt != byteBoundary) {
    for (int i = 0; i < amt; i++) {
      put(0);
    }
  }
  
  updateEndPos();
}

int TBufStream::size() const {
  return endPos_;
}
  
int TBufStream::capacity() const {
  return data_.size();
}

void TBufStream::setCapacity(int capacity__) {
  int sizeDiff = capacity__ - data_.size();
  
  // ignore requests to lower capacity
  if (sizeDiff > 0) {
    data_.resize(capacity__);
    // if new cap greater than old, zero new data
    std::memset(data_.data() + data_.size() - sizeDiff, 0, sizeDiff);
  }
}
  
void TBufStream::writeFrom(TStream& ifs, int sz) {
  recapIfNeeded(endPos_ + sz);
  
  ifs.read((char*)(data_.data() + pos_), sz);
  pos_ += sz;
  
  updateEndPos();
}

void TBufStream::writeTo(TStream& ofs, int sz) {
  ofs.write((char*)(data_.data() + pos_), sz);
  pos_ += sz;
}

void TBufStream::fromString(const std::string& str) {
  data_.resize(str.size());
  memcpy((char*)data_.data(), str.c_str(), str.size());
  pos_ = 0;
  endPos_ = data_.size();
}

void TBufStream::setEndPos(int endPos__) {
  endPos_ = endPos__;
}

TArray<char>& TBufStream::data() {
  return data_;
}

const TArray<char>& TBufStream::data() const {
  return data_;
}

void TBufStream::updateEndPos() {
  if (pos_ > endPos_) endPos_ = pos_;
}

void TBufStream::recapIfNeeded(int targetAmount) {
  if (targetAmount > capacity()) setCapacity(targetAmount * 2);
}


}
