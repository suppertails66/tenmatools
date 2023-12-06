#include "discaster/StdInputStream.h"
#include <cstddef>

namespace Discaster {

    
  StdInputStream::StdInputStream(std::istream& ifs__)
    : InputStream(),
      ifs_(ifs__),
      lineNum_(1) { }
      
  StdInputStream::~StdInputStream() { }

  char StdInputStream::get() {
    char next = ifs_.get();
    
    if (next == '\n') {
      ++lineNum_;
    }
    
    return next;
  }
    
  char StdInputStream::peek() {
    return ifs_.peek();
  }

  void StdInputStream::unget() {
    ifs_.unget();
    
    // Ensure line count remains in sync if a line break is put back
    if (ifs_.peek() == '\n') {
      --lineNum_;
    }
  }

  bool StdInputStream::eof() const {
  //  return ifs_.eof();
    // Treat any stream failure as EOF
    return ifs_.eof() || ifs_.fail();
  }
    
  int StdInputStream::lineNum() const {
    return lineNum_;
  }


} 
