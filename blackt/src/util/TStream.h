#ifndef TSTREAM_H
#define TSTREAM_H


#include "util/EndiannessType.h"
#include "util/SignednessType.h"
#include <string>

namespace BlackT {


class TStream {
public:
  virtual char get() =0;
  virtual void unget() =0;
  virtual char peek() =0;
  virtual void put(char c) =0;
  virtual void read(char* dst, int size) =0;
  virtual void write(const char* src, int size) =0;
  virtual bool good() const =0;
  virtual bool bad() const =0;
  virtual bool fail() const =0;
  virtual bool eof() const =0;
  virtual void clear() =0;
  virtual int tell() =0;
  virtual void seek(int pos) =0;
  virtual int size() =0;
  
  virtual int remaining();
  virtual void seekoff(int offset);
  virtual int readInt(
              int sz = 2,
              EndiannessTypes::EndiannessType end = EndiannessTypes::little,
              SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  virtual void writeInt(
              int value,
              int sz = 2,
              EndiannessTypes::EndiannessType end = EndiannessTypes::little,
              SignednessTypes::SignednessType sign = SignednessTypes::nosign);
  virtual void readCstr(char* dst, int limit = -1);
  virtual void writeCstr(const char* src, int limit = -1);
  virtual void readCstrString(std::string& dst, int limit = -1);
  virtual void readRev(char* dst, int size);
  virtual void writeRev(const char* src, int size);
  virtual void alignToBoundary(int byteBoundary);
  virtual void alignToReadBoundary(int byteBoundary);
  virtual void alignToWriteBoundary(int byteBoundary);
  virtual void padToSize(int sz, char fillChar = 0xFF);
  virtual bool nextIsEof();
  virtual void write(const std::string& str);
  virtual void writeString(const std::string& str);
  virtual void getLine(std::string& dst);
  virtual void skipSpace();
  
  // aliases of readInt() and writeInt() for common types
  virtual int reads8le();
  virtual int reads16le();
  virtual int reads32le();
  virtual int reads64le();
  virtual int readu8le();
  virtual int readu16le();
  virtual int readu32le();
  virtual int readu64le();
  virtual int reads8be();
  virtual int reads16be();
  virtual int reads32be();
  virtual int reads64be();
  virtual int readu8be();
  virtual int readu16be();
  virtual int readu32be();
  virtual int readu64be();
  // consecutive little-endian big-endian format, as used in e.g. CD-ROM
  virtual int reads16lebe();
  virtual int reads32lebe();
  virtual int reads64lebe();
  virtual int readu16lebe();
  virtual int readu32lebe();
  virtual int readu64lebe();
  virtual void writes8le(int value);
  virtual void writes16le(int value);
  virtual void writes32le(int value);
  virtual void writes64le(int value);
  virtual void writeu8le(int value);
  virtual void writeu16le(int value);
  virtual void writeu32le(int value);
  virtual void writeu64le(int value);
  virtual void writes8be(int value);
  virtual void writes16be(int value);
  virtual void writes32be(int value);
  virtual void writes64be(int value);
  virtual void writeu8be(int value);
  virtual void writeu16be(int value);
  virtual void writeu32be(int value);
  virtual void writeu64be(int value);
  // consecutive little-endian big-endian format, as used in e.g. CD-ROM
  virtual void writes16lebe(int value);
  virtual void writes32lebe(int value);
  virtual void writes64lebe(int value);
  virtual void writeu16lebe(int value);
  virtual void writeu32lebe(int value);
  virtual void writeu64lebe(int value);
  virtual int readu8();
  virtual int reads8();
  virtual void writeu8(int value);
  virtual void writes8(int value);
protected:
  TStream();
  virtual ~TStream();
};


}


#endif
