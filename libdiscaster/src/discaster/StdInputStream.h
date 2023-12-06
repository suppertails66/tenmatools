#ifndef DISCASTERSTDINPUTSTREAM_H
#define DISCASTERSTDINPUTSTREAM_H


#include "discaster/InputStream.h"
#include <iostream>

namespace Discaster {


  /**
   * Implementation of InputStream for std::istream.
   */
  class StdInputStream : public InputStream {
  public:
    StdInputStream(std::istream& ifs__);
    virtual ~StdInputStream();
    
    virtual char get();
    
    virtual char peek();
    
    virtual void unget();
    
    virtual bool eof() const;
    
    virtual int lineNum() const;
  protected:
    std::istream& ifs_;
    
    int lineNum_;
  };

}


#endif
