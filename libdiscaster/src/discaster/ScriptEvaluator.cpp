#include "discaster/ScriptEvaluator.h"
#include "discaster/Object.h"
#include "discaster/Classes.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <cstring>

using namespace BlackT;

namespace Discaster {

  
  void ScriptEvaluator::binaryOperatorException(
      ParseNode* left, ParseNode* right, ParseNode* op) {
      throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::binaryOperatorException("
                             "ParseNode*, ParseNode*,"
                             "ParseNode*)",
                             std::string("Line ")
                             + TStringConversion::toString(left->lineNum())
                             + ": "
                             + "Invalid operands to operator "
                             + op->type()
                             + ": "
                             + left->type()
                             + " and "
                             + right->type());
  }

  void ScriptEvaluator::evaluateProgram(ParseNode& parseTree) {
    // Create global environment.
    // The "environment" is simply a "blob" object. Created variables become
    // members of the object.
    // Because we are using a highly simplified model of computation --
    // no user-defined functions or structs, no scoping --
    // we don't have to worry about things like extending the environment,
    // collecting garbage, etc.
    // We just store everything in this global environment and destroy it
    // when we're done.
    Environment globalEnv = BlobObject("environment");
    
    // Add built-ins
    // ...
    
    // Evaluate program
    ParseNode* result = eval(&parseTree, &globalEnv);
    delete result;
  
/*    // Create global environment
    Environment* globalEnv = new Environment();
    
    // Add built-ins
    
    addBuiltIn(*globalEnv, "print", (void*)(&evalPrint));
    addBuiltIn(*globalEnv, "println", (void*)(&evalPrintLn));
    addBuiltIn(*globalEnv, "prettyPrint", (void*)(&evalPrettyPrint));
    addBuiltIn(*globalEnv, "Array", (void*)(&evalArray));
    addBuiltIn(*globalEnv, "arrayGet", (void*)(&evalGetArray));
    addBuiltIn(*globalEnv, "arraySet", (void*)(&evalSetArray));
    addBuiltIn(*globalEnv, "arrayDelete", (void*)(&evalDeleteArray));
  //  addBuiltIn(*globalEnv, "swap", (void*)(&evalSwap));
  //  addBuiltIn(*globalEnv, "copy", (void*)(&evalCopy));
    addBuiltIn(*globalEnv, "error", (void*)(&evalError));
    addBuiltIn(*globalEnv, "readStdInput", (void*)(&evalReadStdInput));
    addBuiltIn(*globalEnv, "stringLength", (void*)(&evalStringLength));
    addBuiltIn(
      *globalEnv, "getStringCharacter", (void*)(&evalGetStringCharacter));
    
    // Evaluate program
    ParseNode* result = eval(&parseTree, globalEnv);
    
  //  cout << *result << endl;
    
    delete result;
    delete globalEnv; */
  }
  
  ParseNode* ScriptEvaluator::eval(ParseNode* pt, Environment* env) {
    if (pt->isType("PROGRAM")) {
      return eval(pt->right(), env);
    }
    else if (pt->isType("STATEMENTS")) {
      delete eval(pt->left(), env);
      // return the value of the last statement
      return eval(pt->right(), env);
    }
    else if (pt->isType("STATEMENT")) {
      return eval(pt->right(), env);
    }
    else if (pt->isType("PAREN_EXPRESSION")) {
      return evalParenExpression(pt, env);
    }
    else if (pt->isType("EXPRESSION_BINARY")) {
      return evalExpressionBinary(pt, env);
    }
/*    else if (pt->isType("UPLUS")) {
      return evalUplus(pt, env);
    }
    else if (pt->isType("UMINUS")) {
      return evalUminus(pt, env);
    }
    else if (pt->isType("UEXCLAMATION_MARK")) {
      return evalUexclamationMark(pt, env);
    } */
    // TODO: update to use new initializer parse tree format
    else if (pt->isType("INITIALIZER")) {
      return evalInitializer(pt, env);
    }
    else if (pt->isType("VARIABLE")) {
      return evalVariable(pt, env);
    }
    else if (pt->isType("BLOCK")) {
      return evalBlock(pt, env);
    }
/*    else if (pt->isType("IF")) {
      return evalIf(pt, env);
    }
    else if (pt->isType("WHILE")) {
      return evalWhile(pt, env);
    }
    else if (pt->isType("FUNCTION")) {
      return evalFunction(pt, env);
    }
    else if (pt->isType("STRUCT")) {
      return evalStruct(pt, env);
    } */
    else if (pt->isType("FUNCTION_CALL")) {
      return evalFunctionCall(pt, env);
    }
    else if (pt->isType("DOT_ACCESS")) {
      return evalDotAccess(pt, env);
    }
    else if (pt->isType("INTEGER")) {
      return new ParseNode(*pt);
    }
    else if (pt->isType("STRING")) {
      return new ParseNode(*pt);
    }
/*    else if (pt->isType("ARRAY")) {
      return new ParseNode(*pt);
    } */
    else {
      throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::eval("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Malformed expression (type: "
                             + pt->type()
                             + ")");
    }
  }
  
  ParseNode* ScriptEvaluator
    ::evalParenExpression(ParseNode* pt, Environment* env) {
    return eval(pt->right(), env);
  }
  
  ParseNode* ScriptEvaluator
    ::evalInitializer(ParseNode* pt, Environment* env) {
    ParseNode* value = NULL;
  
    // do not allow redefinitions of existing variables
    std::string name = pt->left()->stringValue();
    if (env->hasMember(name)) {
      throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::evalInitializer("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Redefinition of variable \""
                             + name
                             + "\"");
    }
    
    // get type name
    std::string typeName = pt->stringValue();
    
    // create object of specified type
    Object obj;
    bool typeExists = false;
    for (int i = 0; i < MasterClassTable::numEntries(); i++) {
      if (strcmp(MasterClassTable::entry(i).name, typeName.c_str()) == 0) {
        obj = MasterClassTable::entry(i).constructor();
        
        typeExists = true;
        break;
      }
    }
    
    if (!typeExists) {
      throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::evalInitializer("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Unknown type \""
                             + typeName
                             + "\"");
    }
    
    // user-assigned initial value
    if (pt->right() != NULL) {
      value = eval(pt->right(), env);
      obj.fromParseNode(*value);
      delete value;
    }
    // default initialization
//    else {
//      value = new ParseNode("INTEGER", pt->lineNum());
//    }
    
    // add to environment
//    env->insert(pt->left()->stringValue(), *value);
    
    // add to environment
    env->setMember(name, obj);
    
    // return pointer
    value = new ParseNode("OBJECTP", pt->lineNum());
    // FIXME: unsafe pointer?
    value->setVoidp(&obj);
    
//    std::cerr << eval(pt->left(), env)->type() << std::endl;
    
    return value;
  }
  
  ParseNode* ScriptEvaluator::evalVariable(
      ParseNode* pt, Environment* env) {
    std::string varName = pt->stringValue();
    if (!env->hasMember(varName)) {
      throw TGenericException(T_SRCANDLINE,
                             "Evaluator::evalVariable("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Variable '"
                             + varName
                             + "' not found in environment \""
                             + env->stringValue()
                             + "\"");
    }
    
    ParseNode* result = 
      new ParseNode("OBJECTP", pt->lineNum());
    // FIXME: unsafe pointer?
    result->setVoidp((void*)&(env->getMember(varName)));
    
    return result;
  }
  
  ParseNode* ScriptEvaluator::evalBlock(
      ParseNode* pt, Environment* env) {
    if (pt->right() != NULL) {
      return eval(pt->right(), env);
    }
    else {
      return new ParseNode("INTEGER", pt->lineNum());
    }
  }
  
  ParseNode* ScriptEvaluator::evalDotAccess(
      ParseNode* pt, Environment* env) {
      
    // FIXME: unsafe pointers?
    
    // for each access:
    // 1. evaluate variable in current environment. It should be an OBJECTP
    // (if there is a sub-access) or a variable (if it is the innermost access)
    // 2. retrieve environment
    // 3. return evaluation of the next dot access in the retrieved environment
      
    // rightmost access
    if (pt->left() == NULL) {
      // do not evaluate yet!
      ParseNode* left = pt->right();
      
      if (!(left->isType("VARIABLE"))) {
        throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::evalDotAccess("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Invalid dot access of "
                             + TStringConversion::toString(left->type())
                             + " instead of VARIABLE");
      }
      
      ParseNode* result = eval(left, env);
      
//      std::cerr << "x: " << ((Object*)result->voidp())->type() << std::endl;
//      std::cerr << "x: " << env->type() << std::endl;
      
      // save environement pointer (pointer to containing object),
      // which is needed for function calls
      result->setFvp((void*)env);
      
      return result;
    }
    // intermediate access
    else {
      ParseNode* left = eval(pt->right(), env);
    
      if (!(left->isType("OBJECTP"))) {
        throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::evalDotAccess("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Invalid use of dot operator on "
                             + TStringConversion::toString(left->type())
                             + " instead of OBJECTP");
      }
      
      // retrieve environment
      Environment* newEnv
        = (Environment*)(left->voidp());
      
      // FIXME: memory leak?
      
      // evaluate sub-access in closure environment
      return eval(pt->left(), newEnv);
    }
  }
    
  ParseNode* ScriptEvaluator::evalDotAccessAssignment(
      ParseNode* pt, Environment* env, ParseNode* assignValue) {
    // rightmost access
    if (pt->left() == NULL) {
      // do not evaluate yet!
      ParseNode* left = pt->right();
      
      if (!(left->isType("VARIABLE"))) {
        throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::evalDotAccessAssignment("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Invalid dot access of "
                             + TStringConversion::toString(left->type())
                             + " instead of VARIABLE");
      }
      
//      env->update(left->stringValue(),
//                  *assignValue);
      Object newObj;
      newObj.fromParseNode(*assignValue);
      env->setMember(left->stringValue(), newObj);
    
//      std::cerr << env->getMember(left->stringValue()).intValue() << std::endl;
      
      return new ParseNode(*assignValue);
    }
    // intermediate access
    else {
    
      ParseNode* left = eval(pt->right(), env);
    
      if (!(left->isType("OBJECTP"))) {
        throw TGenericException(T_SRCANDLINE,
                             "ScriptEvaluator::evalDotAccessAssignment("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Invalid use of dot operator on "
                             + TStringConversion::toString(left->type())
                             + " instead of ENVIRONMENTP");
      }
      
      // retrieve environment
      Environment* newEnv
        = (Environment*)(left->voidp());
      
      // evaluate sub-access in environment
      return evalDotAccessAssignment(pt->left(), newEnv, assignValue);
    }
  }
  
  ParseNode* ScriptEvaluator::evalExpressionBinary(
      ParseNode* pt, Environment* env) {
    ParseNode* op = pt->left();
    
/*    if (op->isType("PLUS")) {
      return evalPlus(pt, env);
    }
    else if (op->isType("MINUS")) {
      return evalMinus(pt, env);
    }
    else if (op->isType("STAR")) {
      return evalStar(pt, env);
    }
    else if (op->isType("FORWARD_SLASH")) {
      return evalForwardSlash(pt, env);
    }
    else if (op->isType("EQUALSEQUALS")) {
      return evalEqualsEquals(pt, env);
    }
    else if (op->isType("NOT_EQUALS")) {
      return evalNotEquals(pt, env);
    } */
    if (op->isType("EQUALS")) {
      return evalEquals(pt, env);
    }
/*    else if (op->isType("LEFT_ANGLEBRACKET")) {
      return evalLeftAngleBracket(pt, env);
    }
    else if (op->isType("RIGHT_ANGLEBRACKET")) {
      return evalRightAngleBracket(pt, env);
    }
    else if (op->isType("LEFT_ANGLEBRACKETEQUALS")) {
      return evalLeftAngleBracketEquals(pt, env);
    }
    else if (op->isType("RIGHT_ANGLEBRACKETEQUALS")) {
      return evalRightAngleBracketEquals(pt, env);
    }
    else if (op->isType("AMPERSAND")) {
      return evalAmpersand(pt, env);
    }
    else if (op->isType("PIPE")) {
      return evalPipe(pt, env);
    } */
    else {
      throw TGenericException(T_SRCANDLINE,
                             "SriptEvaluator::evalExpressionBinary("
                             "ParseNode*, Environment*)",
                             std::string("Line ")
                             + TStringConversion::toString(pt->lineNum())
                             + ": "
                             + "Invalid binary operator (type: "
                             + pt->type()
                             + ")");
    }
  }
  
  ParseNode* ScriptEvaluator::evalEquals(
      ParseNode* pt, Environment* env) {
    ParseNode* left = pt->right()->left();
    ParseNode* right = eval(pt->right()->right(), env);
    
    ParseNode* result = NULL;
    
    // simple variable assignment
    if (left->isType("VARIABLE")) {
      result = new ParseNode(*right);
//      env->update(left->stringValue(),
//                  *result);
      Object newObj;
      newObj.fromParseNode(*result);
      env->setMember(left->stringValue(), newObj);
    }
    // dot access assignment (evaluation required)
    else if (left->isType("DOT_ACCESS")) {
      result = evalDotAccessAssignment(left, env, right);
    }
    else {
      binaryOperatorException(left, right, pt->left());
    }
    
  //  delete left;
    delete right;
    
    return result;
  }
  
  ParseNode* ScriptEvaluator::evalFunctionCall(
      ParseNode* pt, Environment* env) {
    ParseNode* result = NULL;
    ParseNode* functionObjectNode = NULL;
    ParseNode* left = pt->left();
    
//      std::cerr << *pt << std::endl;
//      pt->prettyPrint(std::cerr);
//      std::cerr << std::endl;
    
    // The left node should always be a variable (though technically, a
    // function could return a function that is then called by another
    // function, but I have no plans of doing so).
    // In any case, evaluating it should always result in an OBJECTP
    // or a BUILTIN.
    functionObjectNode = eval(left, env);
    
    // Evaluate function arguments
    NodeCollection args;
    evaluateFunctionCallArgs(args, pt->right(), env);
    
    // Convert argument nodes to Objects
    ObjectArray argObjects;
    for (NodeCollection::iterator it = args.begin();
         it != args.end();
         ++it) {
      Object argObject;
      argObject.fromParseNode(*(*it));
      argObjects.push_back(argObject);
    }
      
/*    if (functionObject->isType("BUILTIN")) {
      // cast the evaluation function pointer to the appropriate type and call it
      return ((*(ParseNode* (*)(NodeCollection&,ParseNode*,Environment*))
             (functionObject->fvp())))
             (args, pt, env);
    }
    else*/ if (!(functionObjectNode->isType("OBJECTP"))) {
      throw TGenericException(T_SRCANDLINE,
                           "ScriptEvaluator::evalFunctionCall("
                           "ParseNode*, Environment*)",
                           std::string("Line ")
                           + TStringConversion::toString(pt->lineNum())
                           + ": "
                           + "Attempted to call "
                           + TStringConversion::toString(left->type())
                           + " as a function");
    }
    
    // Get target Object
//    Object& object = *(Object*)functionObjectNode->voidp();
//    Object& object = *env;
    Object& object = *(Object*)functionObjectNode->fvp();
    
//    std::cerr << "envname: " << object.type() << std::endl;

    // Get the target Object, which should be a function pointer container
    Object& funcObj
      = *(Object*)functionObjectNode->voidp();
    
    if (funcObj.type().compare("functionpointer") != 0) {
      throw TGenericException(T_SRCANDLINE,
                           "ScriptEvaluator::evalFunctionCall("
                           "ParseNode*, Environment*)",
                           std::string("Line ")
                           + TStringConversion::toString(pt->lineNum())
                           + ": "
                           + "Attempted to call "
                           + funcObj.type()
                           + " as a function");
    }
    
    // Retrieve the actual function pointer
    ScriptFunctionPointer funcP = funcObj.functionPointerValue();
    
    // Call the function pointer
//    result = eval(functionObject->right()->right(), &extendedEnvironment);
    try {
      result = funcP(env, &object, argObjects);
    }
    catch (TGenericException& e) {
      throw TGenericException(T_SRCANDLINE,
                           "ScriptEvaluator::evalFunctionCall("
                           "ParseNode*, Environment*)",
                           std::string("Line ")
                           + TStringConversion::toString(pt->lineNum())
                           + ": "
                           + "Error occurred in function call: "
                           + e.problem());
    }
    catch (TException& e) {
      throw TGenericException(T_SRCANDLINE,
                           "ScriptEvaluator::evalFunctionCall("
                           "ParseNode*, Environment*)",
                           std::string("Line ")
                           + TStringConversion::toString(pt->lineNum())
                           + ": "
                           + "Error occurred in function call: "
                           + e.what());
    }
    catch (std::exception& e) {
      throw TGenericException(T_SRCANDLINE,
                           "ScriptEvaluator::evalFunctionCall("
                           "ParseNode*, Environment*)",
                           std::string("Line ")
                           + TStringConversion::toString(pt->lineNum())
                           + ": "
                           + "std::exception occurred in function call: "
                           + e.what());
    }
    
    // Destroy everything we don't need anymore
    for (NodeCollection::iterator it = args.begin();
         it != args.end();
         ++it) {
      delete *it;
    }
    
    return result;
  }
  
  void ScriptEvaluator::evaluateFunctionCallArgs(
      NodeCollection& args,
      ParseNode* pt, Environment* env) {
    while (pt != NULL) {
      ParseNode* node = eval(pt->left(), env);
      args.push_back(node);
      
      pt = pt->right();
    }
  }


}
