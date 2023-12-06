#ifndef DISCASTERLEXEME_H
#define DISCASTERLEXEME_H


#include "discaster/Primitives.h"
#include <iostream>
#include <string>

namespace Discaster {


  /**
   * Generic class representing all types of Discaster lexemes.
   */
  class Lexeme {
  public:
    typedef std::string TypeID;

    Lexeme();
    virtual ~Lexeme();
    Lexeme(TypeID type__,
                int lineNum__);
    Lexeme(TypeID type__,
                int lineNum__,
                DiscasterInt intValue__);
    Lexeme(TypeID type__,
                int lineNum__,
                DiscasterString stringValue__);
    
    TypeID type() const;
    void setType(TypeID type__);
    
    int lineNum() const;
    void setLineNum(int lineNum__);
    
    DiscasterInt intValue() const;
    void setIntValue(DiscasterInt intValue__);
    
    DiscasterString stringValue() const;
    void setStringValue(DiscasterString stringValue__);
    
    /**
     * Returns true if the lexeme is of the given type and false otherwise.
     */
    bool isType(TypeID type__) const;
    
    /**
     * std::ostream operator<< overload for formatted printing.
     */
    friend std::ostream& operator<<(std::ostream& ofs,
                                    const Lexeme& lexeme);
    
  protected:
    
    /**
     * The lexeme's type.
     */
    TypeID type_;
    
    /**
     * The line number from which the lexeme originated.
     */
    int lineNum_;
    
    /**
     * The lexeme's integer value (only applicable to certain types).
     */
    DiscasterInt intValue_;
    
    /**
     * The lexeme's string value (only applicable to certain types).
     */
    DiscasterString stringValue_;
    
  };


}


#endif
