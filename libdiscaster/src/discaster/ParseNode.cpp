#include "discaster/ParseNode.h"
//#include "discaster/Environment.h"
#include <cstddef>

using namespace std;

namespace Discaster {


ParseNode::ParseNode()
  : Lexeme(),
    left_(NULL),
    right_(NULL),
    voidp_(NULL),
    fvp_(NULL) { }

ParseNode::~ParseNode() {
  delete left_;
  delete right_;
}

ParseNode::ParseNode(TypeID type__,
            int lineNum__)
  : Lexeme(type__, lineNum__),
    left_(NULL),
    right_(NULL),
    voidp_(NULL),
    fvp_(NULL) { }
    
/*ParseNode::ParseNode(TypeID type__,
            int lineNum__,
            DiscasterInt intValue__)
  : Lexeme(type__, lineNum__, intValue__),
    left_(NULL),
    right_(NULL) { }
    
ParseNode::ParseNode(TypeID type__,
            int lineNum__,
            DiscasterString stringValue__)
  : Lexeme(type__, lineNum__, stringValue__),
    left_(NULL),
    right_(NULL) { } */

ParseNode::ParseNode(const Lexeme& lexeme)
  : Lexeme(lexeme),
    left_(NULL),
    right_(NULL),
    voidp_(NULL),
    fvp_(NULL) { }

ParseNode::ParseNode(const ParseNode& node)
  : Lexeme((const Lexeme&)node),
    left_(NULL),
    right_(NULL),
    voidp_(node.voidp_),
    fvp_(node.fvp_) {
  if (node.left_ != NULL) {
    left_ = new ParseNode(*(node.left_));
  }
  
  if (node.right_ != NULL) {
    right_ = new ParseNode(*(node.right_));
  }
  
//  voidp_ = node.voidp_;
}

/*ParseNode::ParseNode(ParseNode* node)
  : Lexeme(node),
    left_(NULL),
    right_(NULL) {
  if (node != NULL) {
    left_ = new ParseNode(node.left_);
    right_ = new ParseNode(node.right_);
  }
} */

ParseNode* ParseNode::left() {
  return left_;
}

void ParseNode::setLeft(ParseNode* left__) {
  left_ = left__;
}

ParseNode* ParseNode::right() {
  return right_;
}

void ParseNode::setRight(ParseNode* right__) {
  right_ = right__;
}
  
void ParseNode::prettyPrint(std::ostream& ofs) {
  prettyPrintStep(ofs, 0);
}
  
void* ParseNode::voidp() {
  return voidp_;
}

void ParseNode::setVoidp(void* voidp__) {
  voidp_ = voidp__;
}
  
void* ParseNode::fvp() {
  return fvp_;
}

void ParseNode::setFvp(void* fvp__) {
  fvp_ = fvp__;
}
  
std::ostream& operator<<(std::ostream& ofs,
                         const ParseNode& node) {
  ofs << "ParseNode: "
    << (const Lexeme&)(node)
    << '\t' << "Left: " << (node.left_ != NULL) << endl
    << '\t' << "Right: " << (node.right_ != NULL);
    
/*  if (node.left_ != NULL) {
    ofs << *(node.left_) << endl;
  }
    
  if (node.right_ != NULL) {
    ofs << *(node.right_) << endl;
  } */
  
  return ofs;
}

void ParseNode::printIndentation(std::ostream& ofs,
                                      int indent,
                                      bool linebreaks) {
  if (linebreaks) {
    for (int i = 0; i < indent; i++) {
      ofs << '\t';
    }
  }
}

void ParseNode::breakLine(std::ostream& ofs, bool linebreaks) {
  if (linebreaks) {
    ofs << endl;
  }
}

void ParseNode::prettyPrintStep(std::ostream& ofs,
                                     int indent,
                                     bool linebreaks) {
  if (isType("PROGRAM")) {
    printIndentation(ofs, indent, linebreaks);
    
    ofs << "/* Program start */";
    breakLine(ofs, linebreaks);
    breakLine(ofs, linebreaks);
    
    printIndentation(ofs, indent, linebreaks);
    right_->prettyPrintStep(ofs, 0);
    
    breakLine(ofs, linebreaks);
    printIndentation(ofs, indent, linebreaks);
    ofs << "/* Program end */" << endl;
    breakLine(ofs, linebreaks);
  }
  else if (isType("STATEMENTS")) {
//    printIndentation(ofs, indent, linebreaks);
    left_->prettyPrintStep(ofs, indent);
    right_->prettyPrintStep(ofs, indent);
  }
  else if (isType("STATEMENT")) {
    // All statements begin a new line (linebreaks parameter permitting)
    // except for blocks
    if (!(right_->isType("BLOCK"))) {
      printIndentation(ofs, indent, linebreaks);
    }
//    left_->prettyPrintStep(ofs, indent);
    right_->prettyPrintStep(ofs, indent);
    
    breakLine(ofs, linebreaks);
  }
  else if (isType("EXPRESSION_BINARY")) {
    ofs << "(";
    // primary
    right_->left()->prettyPrintStep(ofs, indent);
    // operator
    left_->prettyPrintStep(ofs, indent);
    // expression
    right_->right()->prettyPrintStep(ofs, indent);
    ofs << ")";
  }
  else if (isType("FUNCTION_CALL")) {
    // primary
    left_->prettyPrintStep(ofs, indent);
    
    // optExpressionList
    ofs << "(";
    if (right_ != NULL) {
      right_->prettyPrintStep(ofs, indent);
    }
    ofs << ")";
  }
  else if (isType("DOT_ACCESS")) {
    if (right_ != NULL) {
      right_->prettyPrintStep(ofs, indent);
    }
    
    // primary
    if (left_ != NULL) {
      ofs << ".";
      left_->prettyPrintStep(ofs, indent);
    }
//    ofs << ")";
  }
  else if (isType("STRUCT")) {
    ofs << "struct ";
    
    // name
    if (left_ != NULL) {
      left_->prettyPrintStep(ofs, indent);
    }
    
    // body
    right_->prettyPrintStep(ofs, indent);
    
    breakLine(ofs, linebreaks);
  }
//  else if (isType("ENVIRONMENTP")) {
//    ofs << "[OBJECT:  ";
//    
//    // environment
//    ((Environment*)(voidp_))->displayLocal(ofs);
//    
//    ofs << "]";
//    
//    breakLine(ofs, linebreaks);
//  }
  else if (isType("CLOSURE")) {
    ofs << "[CLOSURE: ";
    
    // name
    if (left_ != NULL) {
      left_->prettyPrintStep(ofs, indent);
    }
    
    // params
    right_->left()->prettyPrintStep(ofs, indent, false);
    
    // body
    right_->right()->prettyPrintStep(ofs, indent, false);
    
    ofs << "]";
    
    breakLine(ofs, linebreaks);
  }
  // primaries
  else if (isType("STRING")) {
    ofs << '"' << stringValue_ << '"';
  }
  else if (isType("INTEGER")) {
    ofs << intValue_;
  }
  else if (isType("VARIABLE")) {
    ofs << stringValue_;
  }
  else if (isType("PAREN_EXPRESSION")) {
    ofs << "(";
    right_->prettyPrintStep(ofs, indent);
    ofs << ")";
  }
  else if (isType("FUNCTION")) {
//    ofs << endl;
    breakLine(ofs, linebreaks);
    printIndentation(ofs, indent, linebreaks);
    ofs << "function ";
    
    // check if function is anonymous
    if (left_ != NULL) {
      left_->prettyPrintStep(ofs, indent);
//      ofs << " ";
    }
    
    // optVariableList
    ofs << "(";
    if (right_->left() != NULL) {
      right_->left()->prettyPrintStep(ofs, indent);
    }
    ofs << ") ";
    
    // block
    right_->right()->prettyPrintStep(ofs, indent);
    
//    ofs << endl;
    breakLine(ofs, linebreaks);
  }
  else if (isType("EXPRESSION_LIST")) {
  
    // expression
    left_->prettyPrintStep(ofs, indent);
    
    // expressionList
    if (right_ != NULL) {
      ofs << ", ";
      right_->prettyPrintStep(ofs, indent);
    }
    
  }
  else if (isType("VARIABLE_LIST")) {
  
    // variable
    left_->prettyPrintStep(ofs, indent);
    
    // variableList
    if (right_ != NULL) {
      ofs << ", ";
      right_->prettyPrintStep(ofs, indent);
    }
    
  }
  else if (isType("INITIALIZER")) {
//    ofs << "var ";
    ofs << stringValue_ << " ";
    
    // variable name
    left_->prettyPrintStep(ofs, indent);
    
    // optional initializer expression
    if (right_ != NULL) {
      ofs << " = ";
      right_->prettyPrintStep(ofs, indent);
    }
    
  }
  else if (isType("IF")) {
  
    // condition
    ofs << "if (";
    left_->prettyPrintStep(ofs, indent, false);
    ofs << ") ";
    
    // body
    right_->left()->prettyPrintStep(ofs, indent);
    
    // optElse
    if (right_->right() != NULL) {
      printIndentation(ofs, indent, linebreaks);
      ofs << "else ";
      right_->right()->prettyPrintStep(ofs, indent);
    }
    
  }
  else if (isType("WHILE")) {
  
    // condition
    ofs << "while (";
    left_->prettyPrintStep(ofs, indent, false);
    ofs << ") ";
    
    // body
    right_->prettyPrintStep(ofs, indent);
    
  }
  else if (isType("BLOCK")) {
  
    ofs << "{";
    breakLine(ofs, linebreaks);
    
    if (right_ == NULL) {
      printIndentation(ofs, indent + 1, linebreaks);
//      ofs << endl;
      breakLine(ofs, linebreaks);
    }
    else {
      // increase the indentation level within the block
      right_->prettyPrintStep(ofs, indent + 1);
    }
    
    printIndentation(ofs, indent, linebreaks);
//    ofs << "}" << endl;
    ofs << "}";
  }
  // binary operators
  // gee, wouldn't it have been a good idea to just use the symbols
  // themselves instead of making up names?
  else if (isType("PLUS")) {
    ofs << " + ";
  }
  else if (isType("MINUS")) {
    ofs << " - ";
  }
  else if (isType("STAR")) {
    ofs << " * ";
  }
  else if (isType("FORWARD_SLASH")) {
    ofs << " / ";
  }
  else if (isType("EQUALSEQUALS")) {
    ofs << " == ";
  }
  else if (isType("NOT_EQUALS")) {
    ofs << " != ";
  }
  else if (isType("EQUALS")) {
    ofs << " = ";
  }
  else if (isType("LEFT_ANGLEBRACKET")) {
    ofs << " < ";
  }
  else if (isType("RIGHT_ANGLEBRACKET")) {
    ofs << " > ";
  }
  else if (isType("LEFT_ANGLEBRACKETEQUALS")) {
    ofs << " <= ";
  }
  else if (isType("RIGHT_ANGLEBRACKETEQUALS")) {
    ofs << " >= ";
  }
  else if (isType("AMPERSAND")) {
    ofs << " & ";
  }
  else if (isType("PIPE")) {
    ofs << " | ";
  }
  // unary operators
  else if (isType("UPLUS")) {
    ofs << "(+";
    right_->prettyPrintStep(ofs, indent);
    ofs << ")";
  }
  else if (isType("UMINUS")) {
    ofs << "(-";
    right_->prettyPrintStep(ofs, indent);
    ofs << ")";
  }
  else if (isType("UEXCLAMATION_MARK")) {
    ofs << "(!";
    right_->prettyPrintStep(ofs, indent);
    ofs << ")";
  }
  else {
    std::cerr << "UNKNOWN PRETTYPRINT TYPE: " << type() << std::endl;
  }
}


}
