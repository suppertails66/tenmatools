#ifndef DISCASTERSCRIPTLEXER_H
#define DISCASTERSCRIPTLEXER_H


#include "discaster/InputStream.h"
#include "discaster/LexicalStream.h"
#include "discaster/Primitives.h"

namespace Discaster {


  /**
   * Primary class for transforming InputStreams to LexicalStreams.
   */
  class ScriptLexer {
  public:
    /**
     * Reads a Discaster program from inputStream and appends lexemes to lexemeStream.
     * The inputStream will be advanced to EOF.
     */
    static void lexProgram(LexicalStream& lexicalStream,
                           InputStream& inputStream);
  protected:

    /**
     * Reads and returns the next lexeme in the inputStream.
     */
    static Lexeme nextLexeme(InputStream& inputStream);
    
    /**
     * Reads and returns an INTEGER lexeme from the inputStream.
     */
    static Lexeme nextIntegerLexeme(InputStream& inputStream);
    
    /**
     * Reads and returns a STRING lexeme from the inputStream.
     */
    static Lexeme nextStringLexeme(InputStream& inputStream);
    
    /**
     * Reads and returns a variable or keyword lexeme from the inputStream.
     */
    static Lexeme nextVarKeywordLexeme(InputStream& inputStream);
    
    /**
     * Advances the inputStream past whitespace, comments, etc.
     * This also functions as a primitive preprocessor; if external file
     * inclusions are present in the source, they will be detected and their
     * content added to the lexicalStream.
     */
    static void skipWhitespace(LexicalStream& lexicalStream,
                               InputStream& inputStream);
    
    /**
     * Advances the inputStream past the next occurence of the given symbol.
     */
    static void skipSymbol(InputStream& inputStream,
                           char symbol);
    
    /**
     * Advances the inputStream past the next occurence of the given symbol.
     * The read input is placed in dst. The symbol is not included in the
     * output.
     */
    static void readToSymbol(DiscasterString& dst,
                             InputStream& inputStream,
                             char symbol);
    
    static bool isHexDigit(char c);
    
  };


}


#endif
