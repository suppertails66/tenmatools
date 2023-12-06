#ifndef DISCASTERSCRIPTEVALUATOR_H
#define DISCASTERSCRIPTEVALUATOR_H


#include "discaster/ParseNode.h"
#include "discaster/Object.h"
#include <string>
#include <list>
#include <iostream>

namespace Discaster {


  class ScriptEvaluator {
  public:
    void evaluateProgram(ParseNode& parseTree);
  protected:
    typedef std::list<ParseNode*> NodeCollection;
    typedef Object Environment;
  
    void binaryOperatorException(
      ParseNode* left, ParseNode* right, ParseNode* op);
    
//    ParseNode* eval(ParseNode* pt, Environment* env);
    ParseNode* eval(ParseNode* pt, Environment* env);
  
    ParseNode* evalParenExpression(ParseNode* pt, Environment* env);
    ParseNode* evalInitializer(ParseNode* pt, Environment* env);
    ParseNode* evalVariable(ParseNode* pt, Environment* env);
    ParseNode* evalBlock(ParseNode* pt, Environment* env);
    ParseNode* evalDotAccess(ParseNode* pt, Environment* env);
  
    // A special version of evalDotAccess for handling assignment.
    // It's obviously inelegant, but this is the easiest way to skirt the right-
    // associativity issues caused by sequential dot accesses.
    ParseNode* evalDotAccessAssignment(
      ParseNode* pt, Environment* env,
      ParseNode* assignValue);
      
    // A special version of evalDotAccess for dot-access function calls
    // It's not enough to just have the function pointer; we also need
    // the object with which it's associated.
//    ParseNode* evalDotAccessFunctionCall(ParseNode* pt, Environment* env);
    
    ParseNode* evalExpressionBinary(ParseNode* pt, Environment* env);
    ParseNode* evalEquals(ParseNode* pt, Environment* env);
    ParseNode* evalFunctionCall(ParseNode* pt, Environment* env);
    
    void evaluateFunctionCallArgs(
      NodeCollection& args, ParseNode* pt, Environment* env);
    
  };


}


#endif
