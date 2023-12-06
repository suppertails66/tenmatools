#ifndef TBUFSTREAM_H
#define TBUFSTREAM_H


#include "util/TStream.h"
#include "util/TArray.h"
#include <string>

namespace BlackT {


class TBufStream : public TStream {
public:
  // sz = initial buffer capacity.
  // after multiple years of having to set this explicitly, i finally
  // added the necessary 10 lines of code to not have to deal with it!
  // wonderful!!
  // but since it's now set explicitly in hundreds of places, we're stuck with
  // this parameter.
  TBufStream(int sz = 8);
  virtual ~TBufStream();
  
  virtual void open(const char* filename);
  virtual void save(const char* filename);
  
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
  
  virtual void alignToBoundary(int byteBoundary);
  
  // since this stream uses a fixed-size buffer, we can provide a const version
  // of size()
  virtual int size() const;
  virtual int capacity() const;
  virtual void setCapacity(int capacity__);
  virtual void writeFrom(TStream& ifs, int sz);
  virtual void writeTo(TStream& ofs, int sz);
  virtual void fromString(const std::string& str);
  virtual void setEndPos(int endPos__);
  
  TArray<char>& data();
  const TArray<char>& data() const;
  
protected:
  virtual void updateEndPos();
  
  /**
   * Check if we have enough room for the given amount of data, and expand
   * capcity if not.
   */
  virtual void recapIfNeeded(int targetAmount);

  TArray<char> data_;
  int pos_;
  int endPos_;
};


}


#endif
