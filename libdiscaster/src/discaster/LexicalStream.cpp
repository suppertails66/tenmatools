#include "discaster/LexicalStream.h"
#include "exception/TGenericException.h"

namespace Discaster {


  LexicalStream::LexicalStream()
    : getPos_(0) { }
    
  Lexeme LexicalStream::get() {
    if (getPos_ >= lexemes_.size()) {
      throw BlackT::TGenericException(T_SRCANDLINE,
                            "LexicalStream::get()",
                            "getPos_ >= lexemes_.size()");
    }
    
    return lexemes_[getPos_++];
    
    // Advance stream only if more input exists
  //  if (getPos_ < (lexemes_.size() - 1)) {
  //    ++getPos_;
  //  }
  }

  Lexeme LexicalStream::peek() {
    Lexeme lexeme = get();
    unget();
    return lexeme;
  }

  void LexicalStream::unget() {
    if (getPos_ <= 0) {
      throw BlackT::TGenericException(T_SRCANDLINE,
                            "LexicalStream::unget()",
                            "getPos_ <= 0)");
    }
    
    --getPos_;
  }

  void LexicalStream::append(Lexeme lexeme) {
    lexemes_.push_back(lexeme);
  }

  void LexicalStream::unappend() {
    lexemes_.pop_back();
  }

  bool LexicalStream::eof() const {
    if (getPos_ >= lexemes_.size()) {
      return true;
    }
    
    return false;
  }


} 
