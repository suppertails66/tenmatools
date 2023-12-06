#ifndef DISCASTERPARSER_H
#define DISCASTERPARSER_H


#include "discaster/LexicalStream.h"
#include "discaster/ParseNode.h"

namespace Discaster {


/**
 * Discaster parse tree generator.
 * Transforms LexicalStreams into a single Lexeme containing the
 * parse tree for the generated program.
 */
class ScriptParser {
public:
  ScriptParser();
  
  ~ScriptParser();
  
  /**
   * Generates the parse tree for the program in the lexicalStream.
   */
  void parseProgram(LexicalStream& lexicalStream__);
  
  /**
   * Returns a reference to the generated parse tree.
   * Do not call before calling parseProgram().
   */
  ParseNode& parseTree();
  
protected:
  
  /**
   * Pointer to the lexical stream used to generate the parse tree.
   * Ownership is not taken; this exists purely for the convenience of not
   * having to pass the stream around as a parameter to the various
   * parsing functions.
   */
  LexicalStream* lexicalStream_;
  
  /**
   * The generated parse tree.
   * This is owned by the parser and is deleted on destruction.
   */
  ParseNode* parseTree_;

  /**
   * Returns a reference to the lexeme currently being analyzed.
   */
  Lexeme currentLexeme();
  
  /**
   * Returns line number of the current lexeme.
   */
  int currentLineNum();
  
  /**
   * Returns formatted string of current line number.
   */
  std::string currentLineNumString();
  
  /**
   * Returns true only if current lexeme is of the given type.
   */
  bool check(Lexeme::TypeID type);
  
  /**
   * Advances the lexicalStream_.
   */
  Lexeme advance();
  
  /**
   * Advances the lexicalStream_ only if current lexeme is of the given type.
   */
  Lexeme match(Lexeme::TypeID type);

  // Pending and read functions for each nonterminal
  
//  bool programPending();
  ParseNode* program();
  
  bool statementsPending();
  ParseNode* statements();
  
  bool statementPending();
  ParseNode* statement();
  
  bool expressionPending();
  ParseNode* expression();
  
  bool primaryPending();
  ParseNode* primary();
  
  bool operatorPending();
  ParseNode* operator_();
  
//  bool unaryOperatorPending();
//  ParseNode* unaryOperator();
  
  bool optExpressionListPending();
  ParseNode* optExpressionList();
  
  bool expressionListPending();
  ParseNode* expressionList();
  
//  bool functionDefinitionPending();
//  ParseNode* functionDefinition();
  
//  bool structDefinitionPending();
//  ParseNode* structDefinition();
  
  bool functionCallSubPending();
  ParseNode* functionCallSub();
  
  bool functionCallRPending();
  ParseNode* functionCallR();
  
  bool dotAccessListPending();
  ParseNode* dotAccessList();
  
//  bool optVariableListPending();
//  ParseNode* optVariableList();
  
//  bool variableListPending();
//  ParseNode* variableList();
  
  bool blockPending();
  ParseNode* block();
  
  bool optStatementsPending();
  ParseNode* optStatements();
  
  bool initializerPending();
  ParseNode* initializer();
  
//  bool ifStatementPending();
//  ParseNode* ifStatement();
  
//  bool optElsePending();
//  ParseNode* optElse();
  
//  bool whileStatementPending();
//  ParseNode* whileStatement();
  
  
};


}


#endif
