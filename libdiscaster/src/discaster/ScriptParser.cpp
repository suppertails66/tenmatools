#include "discaster/ScriptParser.h"
#include "discaster/Classes.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"
#include <cstddef>
#include <cstring>

namespace Discaster {


ScriptParser::ScriptParser()
  : lexicalStream_(NULL),
    parseTree_(NULL) { };
  
ScriptParser::~ScriptParser() {
  delete parseTree_;
}

void ScriptParser::parseProgram(LexicalStream& lexicalStream__) {
  // Destroy any existing parse tree
  delete parseTree_;
  
  // Remember the lexical stream (purely as a convenience so we don't
  // need to pass it around to all the parse functions)
  lexicalStream_ = &lexicalStream__;
  
  // Parse everything
  parseTree_ = program();
}

ParseNode& ScriptParser::parseTree() {
  return *parseTree_;
}

Lexeme ScriptParser::currentLexeme() {
  return lexicalStream_->peek();
}

int ScriptParser::currentLineNum() {
  return currentLexeme().lineNum();
}

std::string ScriptParser::currentLineNumString() {
  return std::string("Line ")
                     + BlackT::TStringConversion::toString(
                        currentLineNum())
                     + ": ";
}
  
/*bool ScriptParser::programPending() {
  
  if (statementsPending()) {
    return true;
  }
    // Empty programs are not allowed
//    throw BlackT::TGenericException(T_SRCANDLINE,
//                           "ScriptParser::programPending()",
//                           currentLineNumString()
//                             + "Syntax error: Empty program");
  
  return false;
} */

bool ScriptParser::check(Lexeme::TypeID type) {
  if (currentLexeme().isType(type)) {
    return true;
  }
  
  return false;
}

Lexeme ScriptParser::advance() {
  return lexicalStream_->get();
}

Lexeme ScriptParser::match(Lexeme::TypeID type) {
  if (!check(type)) {
    std::string errorString = currentLineNumString();
    errorString += "Syntax error: Expected "
                             + type
                             + ", got "
                             + currentLexeme().type();
    if (!(currentLexeme().stringValue().empty())) {
      errorString += " (identifier: "
                       + currentLexeme().stringValue()
                       + ")";
    }
    throw BlackT::TGenericException(T_SRCANDLINE,
                           "ScriptParser::match(Lexeme::TypeID)",
                           errorString);
  }
  
  return advance();
}

ParseNode* ScriptParser::program() {
  ParseNode* node
    = new ParseNode("PROGRAM", currentLineNum());
  
  node->setRight(statements());

  return node;
}
  
bool ScriptParser::statementsPending() {
  return statementPending();
}

ParseNode* ScriptParser::statements() {
  ParseNode* node = statement();
  
  if (statementsPending()) {
    ParseNode* root
      = new ParseNode("STATEMENTS", currentLineNum());
    root->setLeft(node);
    root->setRight(statements());
    return root;
  }
  else {
    return node;
  }
  
}
  
bool ScriptParser::statementPending() {
//  return expressionPending();
  return expressionPending()
      || initializerPending()
//      || ifStatementPending()
//      || whileStatementPending()
      || blockPending();
}

ParseNode* ScriptParser::statement() {
  ParseNode* node
    = new ParseNode("STATEMENT", currentLineNum());
      
  if (expressionPending()) {
    node->setRight(expression());
  }
  else if (initializerPending()) {
    node->setRight(initializer());
  }
/*  else if (ifStatementPending()) {
    node->setRight(ifStatement());
  }
  else if (whileStatementPending()) {
    node->setRight(whileStatement());
  } */
  else if (blockPending()) {
    node->setRight(block());
  }
  
  return node;
  
}

bool ScriptParser::expressionPending() {
  return primaryPending();
}

ParseNode* ScriptParser::expression() {
  ParseNode* node = primary();
  
  // binary operator
  if (operatorPending()) {
    // operator
    ParseNode* root
      = new ParseNode("EXPRESSION_BINARY", currentLineNum());
    root->setLeft(operator_());
    
    // right expression
    ParseNode* right
      = new ParseNode("EXPRESSION_BINARY_R", currentLineNum());
    right->setLeft(node);
    right->setRight(expression());
    root->setRight(right);
    
    return root;
  }
  // sole primary
  else {
    return node;
  }
}
  
bool ScriptParser::primaryPending() {
  return check("STRING")
          || check("INTEGER")
          || check("VARIABLE")
//          || unaryOperatorPending()
//          || functionDefinitionPending()
//          || structDefinitionPending()
          || check("OPEN_PAREN");
}

ParseNode* ScriptParser::primary() {
  ParseNode* node = NULL;
  
  if (check("STRING")) {
    node = new ParseNode(match("STRING"));
  }
  else if (check("INTEGER")) {
    node = new ParseNode(match("INTEGER"));
  }
  else if (check("VARIABLE")) {
    node = new ParseNode(match("VARIABLE"));
    
    // possible function call
    if (functionCallSubPending()) {
      ParseNode* root
        = functionCallSub();
      
      // attach to deepest level of tree
      ParseNode* finder = root;
      while (finder->left() != NULL) {
        finder = finder->left();
      }
      finder->setLeft(node);
        
      node = root;
    }
    // possible dot access
    else if (check("DOT")) {
      // check for possibility of var.func().var2.var3.func2().var4...
      while (check("DOT")) {
        ParseNode* root
          = new ParseNode("DOT_ACCESS", currentLineNum());
          
        root->setRight(node);
          
        // accessed variable
        match("DOT");
        root->setLeft(dotAccessList());
        
        node = root;
        
        // possible function calls
        ParseNode* callCheck
          = functionCallR();
        if (callCheck != NULL) {
          // attach dot access to deepest level of tree
          ParseNode* finder = callCheck;
          while (finder->left() != NULL) {
            finder = finder->left();
          }
          finder->setLeft(node);
          node = callCheck;
        }
      }
      
    }
    
  }
/*  else if (unaryOperatorPending()) {
    node = unaryOperator();
    node->setRight(primary());
  }
  else if (functionDefinitionPending()) {
    node = functionDefinition();
  }
  else if (structDefinitionPending()) {
    node = structDefinition();
  } */
  else {
    match("OPEN_PAREN");
    node = new ParseNode("PAREN_EXPRESSION", currentLineNum());
    node->setRight(expression());
    match("CLOSE_PAREN");
  }
  
  return node;
}
  
bool ScriptParser::operatorPending() {
  return check("PLUS")
          || check("MINUS")
          || check("STAR")
          || check("FORWARD_SLASH")
          || check("EQUALSEQUALS")
          || check("NOT_EQUALS")
          || check("EQUALS")
          || check("LEFT_ANGLEBRACKET")
          || check("RIGHT_ANGLEBRACKET")
          || check("LEFT_ANGLEBRACKETEQUALS")
          || check("RIGHT_ANGLEBRACKETEQUALS")
          || check("AMPERSAND")
          || check("PIPE");
}

ParseNode* ScriptParser::operator_() {
  ParseNode* node = NULL;
  
  if (check("PLUS")) {
    node = new ParseNode(match("PLUS"));
  }
  else if (check("MINUS")) {
    node = new ParseNode(match("MINUS"));
  }
  else if (check("STAR")) {
    node = new ParseNode(match("STAR"));
  }
  else if (check("FORWARD_SLASH")) {
    node = new ParseNode(match("FORWARD_SLASH"));
  }
  else if (check("EQUALSEQUALS")) {
    node = new ParseNode(match("EQUALSEQUALS"));
  }
  else if (check("NOT_EQUALS")) {
    node = new ParseNode(match("NOT_EQUALS"));
  }
  else if (check("EQUALS")) {
    node = new ParseNode(match("EQUALS"));
  }
  else if (check("LEFT_ANGLEBRACKET")) {
    node = new ParseNode(match("LEFT_ANGLEBRACKET"));
  }
  else if (check("RIGHT_ANGLEBRACKET")) {
    node = new ParseNode(match("RIGHT_ANGLEBRACKET"));
  }
  else if (check("LEFT_ANGLEBRACKETEQUALS")) {
    node = new ParseNode(match("LEFT_ANGLEBRACKETEQUALS"));
  }
  else if (check("RIGHT_ANGLEBRACKETEQUALS")) {
    node = new ParseNode(match("RIGHT_ANGLEBRACKETEQUALS"));
  }
  else if (check("AMPERSAND")) {
    node = new ParseNode(match("AMPERSAND"));
  }
  else {
    node = new ParseNode(match("PIPE"));
  }
  
  return node;
}
  
/*bool ScriptParser::unaryOperatorPending() {
  return check("PLUS")
          || check("MINUS")
          || check("EXCLAMATION_MARK");
}

ParseNode* ScriptParser::unaryOperator() {
  ParseNode* node = NULL;
  
  if (check("PLUS")) {
    match("PLUS");
    node = new ParseNode(
      "UPLUS", currentLineNum());
  }
  else if (check("MINUS")) {
    match("MINUS");
    node = new ParseNode(
      "UMINUS", currentLineNum());
  }
  else {
    match("EXCLAMATION_MARK");
    node = new ParseNode(
      "UEXCLAMATION_MARK", currentLineNum());
  }
  
  return node;
} */
  
bool ScriptParser::optExpressionListPending() {
  return true;
}

ParseNode* ScriptParser::optExpressionList() {
//  ParseNode* root
//    = new ParseNode("OPT_EXPRESSION_LIST", currentLineNum());
  ParseNode* node = NULL;
  
  if (expressionListPending()) {
    node = expressionList();
  }
  
  return node;
}
  
bool ScriptParser::expressionListPending() {
  return expressionPending();
}

ParseNode* ScriptParser::expressionList() {
  ParseNode* node
    = new ParseNode("EXPRESSION_LIST", currentLineNum());
  
  node->setLeft(expression());
  
  if (check("COMMA")) {
    match("COMMA");
    node->setRight(expressionList());
  }
  
  return node;
}
  
/*bool ScriptParser::functionDefinitionPending() {
  return check("FUNCTION");
}

ParseNode* ScriptParser::functionDefinition() {
  
  ParseNode* node
    = new ParseNode(match("FUNCTION"));
  
  // named function
  if (check("VARIABLE")) {
    node->setLeft(new ParseNode(match("VARIABLE")));
  }
  
  node->setRight(
    new ParseNode("FUNCTION_R", currentLineNum()));
  // formal parameters
  match("OPEN_PAREN");
  node->right()->setLeft(optVariableList());
  match("CLOSE_PAREN");
  // body
  node->right()->setRight(block());
  
  return node;
}
  
bool ScriptParser::structDefinitionPending() {
  return check("STRUCT");
}

ParseNode* ScriptParser::structDefinition() {
  
  ParseNode* node
    = new ParseNode(match("STRUCT"));
  
  // named struct
  if (check("VARIABLE")) {
    node->setLeft(new ParseNode(match("VARIABLE")));
  }
  
  // body
  node->setRight(block());
  
  return node;
} */
  
bool ScriptParser::functionCallSubPending() {
  return check("OPEN_PAREN");
}

ParseNode* ScriptParser::functionCallSub() {
  ParseNode* node
    = new ParseNode("FUNCTION_CALL", currentLineNum());

  match("OPEN_PAREN");
  
  // arguments
  node->setRight(optExpressionList());
  
  match("CLOSE_PAREN");
  
  // possible calls of this call
  ParseNode* next = functionCallR();
  // Attach to deepest level of tree.
  // In order to parse these calls, we must evaluate them
  // from left to right. However, our recursive descent approach
  // naturally produces right-to-left evaluation. The simplest
  // way to deal with it is a linear search.
  if (next != NULL) {
    ParseNode* finder = next;
    while (finder->left() != NULL) {
      finder = finder->left();
    }
    finder->setLeft(node);
    // return the root of the tree (outermost call)
    return next;
  }
  else {
    // this is the outermost call, and the root node in the tree
    return node;
  }
}

bool ScriptParser::functionCallRPending() {
  return true;
}

ParseNode* ScriptParser::functionCallR() {
  if (check("OPEN_PAREN")) {
    ParseNode* node
      = new ParseNode("FUNCTION_CALL", currentLineNum());
  
    match("OPEN_PAREN");
    
    // arguments
    node->setRight(optExpressionList());
    
    match("CLOSE_PAREN");
    
    // possible calls of this call
    ParseNode* next = functionCallR();
    // Attach to deepest level of tree.
    // In order to parse these calls, we must evaluate them
    // from left to right. However, our recursive descent approach
    // naturally produces right-to-left evaluation. The simplest
    // way to deal with it is a linear search.
    if (next != NULL) {
      ParseNode* finder = next;
      while (finder->left() != NULL) {
        finder = finder->left();
      }
      finder->setLeft(node);
      // return the root of the tree (outermost call)
      return next;
    }
    else {
      // this is the outermost call, and the root node in the tree
      return node;
    }
  }
/*  else if (check("DOT")) {
//    ParseNode* node
//      = new ParseNode("DOT_ACCESS", currentLineNum());
//    match("DOT");
//    node->setRight(dotAccessList());
    match("DOT");
    ParseNode* node = dotAccessList();
    
    // possible calls of this call
    ParseNode* next = functionCallR();
    // Attach to deepest level of tree.
    // In order to parse these calls, we must evaluate them
    // from left to right. However, our recursive descent approach
    // naturally produces right-to-left evaluation. The simplest
    // way to deal with it is a linear search.
    if (next != NULL) {
      ParseNode* finder = next;
      while (finder->left() != NULL) {
        finder = finder->left();
      }
      finder->setLeft(node);
      // return the root of the tree (outermost call)
      return next;
    }
    else {
      // this is the outermost call, and the root node in the tree
      return node;
    }
  } */
  else {
    return NULL;
  }
}
  
bool ScriptParser::dotAccessListPending() {
  return check("VARIABLE");
}

ParseNode* ScriptParser::dotAccessList() {
  ParseNode* node
    = new ParseNode("DOT_ACCESS", currentLineNum());
  
  node->setRight(new ParseNode(match("VARIABLE")));
  
  // sub-access
  if (check("DOT")) {
    match("DOT");
    node->setLeft(dotAccessList());
  }
  
  return node;
}
  
/*bool ScriptParser::optVariableListPending() {
  return true;
}

ParseNode* ScriptParser::optVariableList() {
  ParseNode* node = NULL;
  
  if (variableListPending()) {
    node = variableList();
  }
  
  return node;
}

bool ScriptParser::variableListPending() {
  return check("VARIABLE");
}

ParseNode* ScriptParser::variableList() {
  ParseNode* node
    = new ParseNode("VARIABLE_LIST", currentLineNum());
  
  node->setLeft(new ParseNode(match("VARIABLE")));
  
  if (check("COMMA")) {
    match("COMMA");
    node->setRight(variableList());
  }
  
  return node;
} */
  
bool ScriptParser::blockPending() {
  return check("OPEN_CURLYBRACE");
}

ParseNode* ScriptParser::block() {
  match("OPEN_CURLYBRACE");
  
  ParseNode* node
    = new ParseNode("BLOCK", currentLineNum());
  
  node->setRight(optStatements());
  
  match("CLOSE_CURLYBRACE");
  return node;
}
  
bool ScriptParser::optStatementsPending() {
  return true;
}

ParseNode* ScriptParser::optStatements() {
  ParseNode* node = NULL;
  
  if (statementsPending()) {
    node = statements();
  }
  
  return node;
}
  
bool ScriptParser::initializerPending() {
  return check("INITIALIZER");

  // formerly, initializers were declared with the "var" keywords, as in
  // "var name [= initialValue]".
  // now, instead of "var", we use the name of a (built-in) class:
  // "IsoFilesystem name [= initialValue]".
  // we thus need to check the variable name against the list of known types.
  
/*  std::string name = currentLexeme().stringValue();
  
  for (int i = 0; i < MasterClassTable::numEntries(); i++) {
    const MasterClassTableEntry& entry = MasterClassTable::entry(i);
    if (strcmp(name.c_str(), entry.name) == 0) {
      return true;
    }
  }
  
  return false; */
}

ParseNode* ScriptParser::initializer() {
  ParseNode* node
    = new ParseNode(match("INITIALIZER"));
    
//  ParseNode* node
//    = new ParseNode("INITIALIZER", currentLineNum());
  
  node->setLeft(new ParseNode(match("VARIABLE")));
  
  // optional initialization value
  if (check("EQUALS")) {
    match("EQUALS");
    node->setRight(expression());
  }
  
  return node;
}
  
/*bool ScriptParser::ifStatementPending() {
  return check("IF");
}

ParseNode* ScriptParser::ifStatement() {
  ParseNode* node
    = new ParseNode(match("IF"));
  
  // condition
  match("OPEN_PAREN");
  node->setLeft(statement());
  match("CLOSE_PAREN");
  
  node->setRight(new ParseNode("IF_R", currentLineNum()));
  
  // body
  node->right()->setLeft(statement());
  
  // optElse
  node->right()->setRight(optElse());
  
  return node;
}
  
bool ScriptParser::optElsePending() {
  return true;
}

ParseNode* ScriptParser::optElse() {
  if (check("ELSE")) {
    match("ELSE");
    return statement();
  }
  else {
    return NULL;
  }
}
  
bool ScriptParser::whileStatementPending() {
  return check("WHILE");
}

ParseNode* ScriptParser::whileStatement() {
  ParseNode* node
    = new ParseNode(match("WHILE"));
  
  // condition
  match("OPEN_PAREN");
  node->setLeft(statement());
  match("CLOSE_PAREN");
  
  // body
  node->setRight(statement());
  
  return node;
} */


} 
