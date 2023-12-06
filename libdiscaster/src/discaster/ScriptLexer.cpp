#include "discaster/ScriptLexer.h"
#include "discaster/StdInputStream.h"
#include "discaster/Classes.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <string>
#include <fstream>
#include <cctype>
#include <cstring>
#include <iostream>

using namespace std;
using namespace BlackT;

namespace Discaster {


  void ScriptLexer::lexProgram(LexicalStream& lexicalStream,
                              InputStream& inputStream) {
    while (!inputStream.eof()) {
      skipWhitespace(lexicalStream, inputStream);
      lexicalStream.append(nextLexeme(inputStream));
    }
  }

  Lexeme ScriptLexer::nextLexeme(InputStream& inputStream) {
    char next = inputStream.get();
    
    if (inputStream.eof()) {
      return Lexeme("EOF", inputStream.lineNum());
    }
    
    // 1-character terminals
    switch (next) {
    case '(':
      return Lexeme("OPEN_PAREN", inputStream.lineNum());
      break;
    case ')':
      return Lexeme("CLOSE_PAREN", inputStream.lineNum());
      break;
    case '.':
      return Lexeme("DOT", inputStream.lineNum());
      break;
    case '=':
      // distinguish between = and ==
//      if (inputStream.get() == '=') {
//        return Lexeme("EQUALSEQUALS", inputStream.lineNum());
//      }
//      else {
//        inputStream.unget();
        return Lexeme("EQUALS", inputStream.lineNum());
//      }
      break;
/*    case '+':
      return Lexeme("PLUS", inputStream.lineNum());
      break;
    case '-':
      return Lexeme("MINUS", inputStream.lineNum());
      break;
    case '*':
      return Lexeme("STAR", inputStream.lineNum());
      break;
    case '/':
      return Lexeme("FORWARD_SLASH", inputStream.lineNum());
      break;
    case '!':
      // distinguish between ! and !=
      if (inputStream.get() == '=') {
        return Lexeme("NOT_EQUALS", inputStream.lineNum());
      }
      else {
        inputStream.unget();
        return Lexeme("EXCLAMATION_MARK", inputStream.lineNum());
      }
      break;
    case '<':
      // distinguish between < and <=
      if (inputStream.get() == '=') {
        return Lexeme("LEFT_ANGLEBRACKETEQUALS", inputStream.lineNum());
      }
      else {
        inputStream.unget();
        return Lexeme("LEFT_ANGLEBRACKET", inputStream.lineNum());
      }
      break;
    case '>':
      // distinguish between > and >=
      if (inputStream.get() == '=') {
        return Lexeme("RIGHT_ANGLEBRACKETEQUALS", inputStream.lineNum());
      }
      else {
        inputStream.unget();
        return Lexeme("RIGHT_ANGLEBRACKET", inputStream.lineNum());
      }
      break;
    case '&':
      return Lexeme("AMPERSAND", inputStream.lineNum());
      break;
    case '|':
      return Lexeme("PIPE", inputStream.lineNum());
      break;
  //  case '"':
  //    return Lexeme("DOUBLEQUOTE");
  //    break;
  //  case '\\':
  //    return Lexeme("BACKSLASH", inputStream.lineNum());
  //    break; */
    case ',':
      return Lexeme("COMMA", inputStream.lineNum());
      break;
  //  case '_':
  //    return Lexeme("UNDERSCORE", inputStream.lineNum());
  //    break;
    case '{':
      return Lexeme("OPEN_CURLYBRACE", inputStream.lineNum());
      break;
    case '}':
      return Lexeme("CLOSE_CURLYBRACE", inputStream.lineNum());
      break;
    default:
      
      break;
    }
    
    // Multi-character terminals
    // Integer literals
    if (isdigit(next)) {
      inputStream.unget();
      return nextIntegerLexeme(inputStream);
    }
    // Keywords and variable identifiers
    else if (isalpha(next)) {
      inputStream.unget();
      return nextVarKeywordLexeme(inputStream);
    }
    // String literals
    else if (next == '"') {
      inputStream.unget();
      return nextStringLexeme(inputStream);
    }
    else {
      throw BlackT::TGenericException(T_SRCANDLINE,
                             "ScriptLexer::nextLexeme(InputStream&)",
                             std::string("Unable to lex symbol '")
                             + next + "' at line "
                             + TStringConversion::toString(
                                  inputStream.lineNum()));
    }
  }

  Lexeme ScriptLexer::nextIntegerLexeme(InputStream& inputStream) {
    DiscasterString intString;
    
    char next = inputStream.get();
    // TODO: proper hex detection
    while (!(inputStream.eof())
           && (isdigit(next)
                 || (tolower(next) == 'x')
                 || (tolower(next) == 'a')
                 || (tolower(next) == 'b')
                 || (tolower(next) == 'c')
                 || (tolower(next) == 'd')
                 || (tolower(next) == 'e')
                 || (tolower(next) == 'f')
               )
           ) {
      intString += next;
      next = inputStream.get();
    }
    inputStream.unget();
    
    DiscasterInt nativeValue = TStringConversion::fromString<int>(intString);
    
    return Lexeme("INTEGER", inputStream.lineNum(), nativeValue);
  }

  Lexeme ScriptLexer::nextStringLexeme(InputStream& inputStream) {
    DiscasterString str;
    
    // skip leading '"'
    inputStream.get();
    
//    readToSymbol(str, inputStream, '"');

    // read strings as UTF-8 (ASCII-compatible)
    // TODO: user-configurable encodings
    while (!(inputStream.eof())) {
      char next = inputStream.get();
      if (next == '"') {
        break;
      }
      else {
        // check for escape sequences
        if (!inputStream.eof() && (next == '\\')) {
          char nextnext = inputStream.get();
          
          // backslash literal
          if (nextnext == '\\') {
            str += nextnext;
            continue;
          }
          // dec value
          else if (!inputStream.eof() && isdigit(nextnext)) {
            std::string numStr = "";
            char next = inputStream.get();
            while (!inputStream.eof() && isdigit(next)) {
              numStr += next;
              next = inputStream.get();
            }
            inputStream.unget();
            
            BlackT::TByte value = TStringConversion::stringToInt(numStr);
            str += (char)value;
            continue;
          }
          // hex value
          else if (!inputStream.eof() && (nextnext == 'x')) {
            std::string numStr = "0x";
            char next = inputStream.get();
            while (!inputStream.eof() && isHexDigit(next)) {
              numStr += next;
              next = inputStream.get();
            }
            inputStream.unget();
            
            BlackT::TByte value = TStringConversion::stringToInt(numStr);
            str += (char)value;
            continue;
          }
          else {
            throw TGenericException(T_SRCANDLINE,
                                    "ScriptLexer::nextStringLexeme()",
                                    std::string("Unknown escape sequence: ")
                                      + nextnext);
          }
          
          // treat as literal
          inputStream.unget();
        }
        
        str += next;
        
        // check for presence of additional UTF-8 bytes
        int trailbytes = 0;
        if (((unsigned char)next & 0xE0) == 0xC0) {
          trailbytes = 1;
        }
        else if (((unsigned char)next & 0xF0) == 0xE0) {
          trailbytes = 2;
        }
        else if (((unsigned char)next & 0xF8) == 0xF0) {
          trailbytes = 3;
        }
        
        for (int i = 0; i < trailbytes; i++) {
          str += inputStream.get();
        }
      }
    }
    
    return Lexeme("STRING", inputStream.lineNum(), str);
  }

  Lexeme ScriptLexer::nextVarKeywordLexeme(InputStream& inputStream) {
    DiscasterString name;
    
    char next = inputStream.get();
    // accept any combination of letters, numbers, and underscores
    // (but must begin with a letter)
    while (!(inputStream.eof())
             && (!isspace(next))
             && (isalpha(next)
                 || isdigit(next)
                 || (next == '_'))) {
      name += next;
      next = inputStream.get();
    }
    inputStream.unget();
    
    // Check for keywords
/*    if (name.compare("function") == 0) {
      return Lexeme("FUNCTION", inputStream.lineNum(), name);
    }
    else if (name.compare("struct") == 0) {
      return Lexeme("STRUCT", inputStream.lineNum(), name);
    }
    else if (name.compare("var") == 0) {
      return Lexeme("VAR", inputStream.lineNum(), name);
    }
    else if (name.compare("if") == 0) {
      return Lexeme("IF", inputStream.lineNum(), name);
    }
    else if (name.compare("else") == 0) {
      return Lexeme("ELSE", inputStream.lineNum(), name);
    }
    else if (name.compare("while") == 0) {
      return Lexeme("WHILE", inputStream.lineNum(), name);
    }
    // Not a keyword: assume variable identifier
    else {
      return Lexeme("VARIABLE", inputStream.lineNum(), name);
    } */
    
    for (int i = 0; i < MasterClassTable::numEntries(); i++) {
      if (strcmp(name.c_str(), MasterClassTable::entry(i).name) == 0) {
        return Lexeme("INITIALIZER", inputStream.lineNum(), name);
      }
    }
    
    return Lexeme("VARIABLE", inputStream.lineNum(), name);
  }

  void ScriptLexer::skipWhitespace(LexicalStream& lexicalStream,
                                  InputStream& inputStream) {
    char next;
    while (!(inputStream.eof())) {
      next = inputStream.get();
      
      // Comments
      if (next == '/') {
        // peek at next character
        char nextnext = inputStream.peek();
        
        // Single-line comment (//)
        if (nextnext == '/') {
          inputStream.get();
          
          // read to end of line
          skipSymbol(inputStream, '\n');
        }
        // Block comment (/* */)
        else if (nextnext == '*') {
          inputStream.get();
          
          char c;
          do {
            // read to next *
            skipSymbol(inputStream, '*');
            
            // get next character
            c = inputStream.get();
            
            // stop at EOF or when "*/" sequence is reached
          } while (!(inputStream.eof()) && (c != '/'));
        }
        // otherwise, not a comment: we're done
        else {
          inputStream.unget();
          break;
        }
        
      }
      // External file inclusion
      else if (next == '@') {
        // Read filename
        char c;
        DiscasterString filename;
        while (!(inputStream.eof())) {
          // get next character
          c = inputStream.get();
          
          // stop when whitespace is reached
          if (isspace(c)) {
            break;
          }
          else {
            filename += c;
          }
        }
        
        // Open file and read lexemes
        std::ifstream ifs(filename.c_str());
        StdInputStream newInputStream(ifs);
        lexProgram(lexicalStream, newInputStream);
        
        // Chop off the end-of-input lexeme so everything gets
        // properly integrated into the final program
        lexicalStream.unappend();
      }
      // If character is not whitespace and does not have some other special
      // meaning, we're done; otherwise, keep looping
      else if (!(isspace(next))) {
        inputStream.unget();
        break;
      }
      
    } 
  }

  void ScriptLexer::skipSymbol(InputStream& inputStream, char symbol) {
    while (!(inputStream.eof())) {
      char next = inputStream.get();
      if (next == symbol) {
        break;
      }
    }
  }

  void ScriptLexer::readToSymbol(DiscasterString& dst,
                                  InputStream& inputStream,
                                  char symbol) {
    dst = "";
    while (!(inputStream.eof())) {
      char next = inputStream.get();
      if (next == symbol) {
        break;
      }
      else {
        dst += next;
      }
    }
  }
  
  bool ScriptLexer::isHexDigit(char c) {
    c = tolower(c);
    if (isdigit(c)
        || (c == 'a')
        || (c == 'b')
        || (c == 'c')
        || (c == 'd')
        || (c == 'e')
        || (c == 'f')) return true;
    
    return false;
  }


}
