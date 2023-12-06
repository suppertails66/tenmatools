#ifndef DISCASTERPARSENODE_H
#define DISCASTERPARSENODE_H


#include "discaster/Lexeme.h"
#include <iostream>

namespace Discaster {


  // forward declaration
  class Object;

  /**
   * A node in a Discaster parse tree.
   * Extends ParseNode with binary tree functionality.
   */
  class ParseNode : public Lexeme {
  public:

    ParseNode();
    virtual ~ParseNode();
    ParseNode(TypeID type__,
                int lineNum__);
  /*  ParseNode(TypeID type__,
                int lineNum__,
                DiscasterInt intValue__);
    ParseNode(TypeID type__,
                int lineNum__,
                DiscasterString stringValue__); */
    ParseNode(const Lexeme& lexeme);
    ParseNode(const ParseNode& node);
  /*  ParseNode(ParseNode* node); */
    
    ParseNode* left();
    void setLeft(ParseNode* left__);
    
    ParseNode* right();
    void setRight(ParseNode* right__);
    
    void* voidp();
    void setVoidp(void* voidp__);
    
    void* fvp();
    void setFvp(void* fvp__);
    
    /**
     * Print "pretty" representation of parse tree.
     */
    void prettyPrint(std::ostream& ofs);
    
    /**
     * Print "ugly" representation of parse tree.
     */
    friend std::ostream& operator<<(std::ostream& ofs,
                                    const ParseNode& node);
    
  protected:
    
    /**
     * Helper for pretty printing that properly indents a line.
     */
    void printIndentation(std::ostream& ofs,
                          int indent,
                          bool linebreaks = true);
    
    /**
     * Helper for pretty printing that breaks a line if needed.
     */
    void breakLine(std::ostream& ofs, bool linebreaks);

    /**
     * Internal implementation of pretty printing.
     */
    void prettyPrintStep(std::ostream& ofs, int indent, bool linebreaks = true);
    
    /**
     * Left child pointer.
     */
    ParseNode* left_;
    
    /**
     * Right child pointer.
     */
    ParseNode* right_;
    
    /**
     * Void pointer.
     * Ownership is not taken when set.
     * Used for storing environment pointers during evaluation.
     */
    void* voidp_;
    
    /**
     * Evaluation function pointer.
     */
    void* fvp_;
    
  };


}


#endif 
