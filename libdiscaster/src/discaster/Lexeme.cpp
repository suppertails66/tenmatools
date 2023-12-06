#include "discaster/Lexeme.h"

namespace Discaster {


  Lexeme::Lexeme()
    : lineNum_(0),
      intValue_(0) { }

  Lexeme::~Lexeme() { }
    
  Lexeme::Lexeme(TypeID type__,
                           int lineNum__)
    : type_(type__),
      lineNum_(lineNum__),
      intValue_(0) { }
      
  Lexeme::Lexeme(TypeID type__,
                           int lineNum__,
                           DiscasterInt intValue__)
    : type_(type__),
      lineNum_(lineNum__),
      intValue_(intValue__) { }
      
  Lexeme::Lexeme(TypeID type__,
                           int lineNum__,
                           DiscasterString stringValue__)
    : type_(type__),
      lineNum_(lineNum__),
      intValue_(0),
      stringValue_(stringValue__) { }
    
  Lexeme::TypeID Lexeme::type() const {
    return type_;
  }

  void Lexeme::setType(TypeID type__) {
    type_ = type__;
  }
    
  int Lexeme::lineNum() const {
    return lineNum_;
  }

  void Lexeme::setLineNum(int lineNum__) {
    lineNum_ = lineNum__;
  }

  DiscasterInt Lexeme::intValue() const {
    return intValue_;
  }

  void Lexeme::setIntValue(DiscasterInt intValue__) {
    intValue_ = intValue__;
  }

  DiscasterString Lexeme::stringValue() const {
    return stringValue_;
  }

  void Lexeme::setStringValue(DiscasterString stringValue__) {
    stringValue_ = stringValue__;
  }

  bool Lexeme::isType(TypeID type__) const {
    if (type_.compare(type__) == 0) {
      return true;
    }
    
    return false;
  }

  std::ostream& operator<<(std::ostream& ofs,
                           const Lexeme& lexeme) {
    ofs << "Lexeme: " << std::endl
        << "\ttype_: " << lexeme.type_ << std::endl
        << "\tlineNum_: " << lexeme.lineNum_ << std::endl
        << "\tintValue_: " << lexeme.intValue_ << std::endl
        << "\tstringValue_: " << lexeme.stringValue_ << std::endl;
    
    return ofs;
  }


} 
