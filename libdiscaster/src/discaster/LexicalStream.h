#ifndef DISCASTERLEXICALSTREAM_H
#define DISCASTERLEXICALSTREAM_H


#include "discaster/Lexeme.h"
#include <vector>

namespace Discaster {


  /**
   * Represents a stream of ordered lexemes.
   * Access is read-write, but new lexemes may only be appended rather than
   * inserted arbitrarily.
   */
  class LexicalStream {
  public:
    LexicalStream();
    
    /**
     * Fetches the next lexeme in the stream and advances the stream.
     */
    Lexeme get();
    
    /**
     * Fetches the next lexeme in the stream without advancing it.
     */
    Lexeme peek();
    
    /**
     * "Undoes" the previous get() operation, restoring the stream's last state.
     */
    void unget();
    
    /**
     * Appends a new lexeme to the stream.
     */
    void append(Lexeme lexeme);
    
    /**
     * Removes the final lexeme from the stream.
     */
    void unappend();
    
    /**
     * Returns true if end of the stream has been reached and false otherwise.
     */
    bool eof() const;
    
  protected:
    typedef std::vector<Lexeme> LexemeCollection;
    
    /**
     * The contained lexemes.
     */
    LexemeCollection lexemes_;
    
    /**
     * Index in lexemes_ of the next lexeme to be fetched by get().
     */
    unsigned int getPos_;
    
  };


}


#endif 
