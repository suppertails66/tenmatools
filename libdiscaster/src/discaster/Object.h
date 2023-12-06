#ifndef DISCASTEROBJECT_H
#define DISCASTEROBJECT_H


#include "discaster/Primitives.h"
#include "discaster/ParseNode.h"
#include <map>
#include <vector>
#include <string>

namespace Discaster {

  // forward declaration
  class Object;
  
  typedef std::vector<Object> ObjectArray;
  typedef std::vector<Object*> ObjectPointerArray;
  typedef std::map<DiscasterString, Object> ObjectMemberMap;
  
  /**
   * Typedef of prototype for object function pointers.
   * @param env The environment in which the function was called.
   *            For now, this will always be the global environment.
   * @param obj The Object on which the function was called.
   * @param args The function's arguments (passed by value).
   * @return A ParseNode containing the function's return value.
   */
  typedef ParseNode* (*ScriptFunctionPointer)(
    Object* env, Object* obj, ObjectArray args);

  /**
   * An object in the scripting system.
   * Objects are simple and "heavyweight": every type of object is represented
   * by this single class, which contains the necessary data fields to represent
   * all of them (simultaneously).
   */
  class Object {
  public:
    
    std::string type() const;
    void setType(const std::string& type__);
    
    DiscasterInt intValue() const;
    void setIntValue(DiscasterInt intValue__);
    
    const DiscasterString& stringValue() const;
    void setStringValue(const DiscasterString& stringValue__);
    
    const ObjectArray& arrayValue() const;
    ObjectArray& arrayValue();
    void setArrayValue(const ObjectArray& arrayValue__);
    
    const ScriptFunctionPointer& functionPointerValue() const;
    void setFunctionPointerValue(
      const ScriptFunctionPointer& functionPointerValue__);
    
    // members getters/setters
    bool hasMember(const DiscasterString& name) const;
    Object& getMember(const DiscasterString& name);
    const Object& getMember(const DiscasterString& name) const;
    void setMember(const DiscasterString& name, const Object& obj);
//    ObjectMemberMap& members();
    const ObjectMemberMap& members() const;
    ObjectMemberMap& members();
    
    // special shortcuts for the case where we want to set a member to a
    // common type
    // i suppose i could set up a cast but eh
    void setMember(const DiscasterString& name, DiscasterInt intValue__);
    void setMember(
        const DiscasterString& name, const DiscasterString& stringValue__);
    
    DiscasterInt getMemberInt(const DiscasterString& name) const;
    DiscasterString getMemberString(const DiscasterString& name) const;
    
    /**
     * Assign to the object from a ParseNode.
     * INTEGER becomes int, STRING becomes string, etc.
     * Nonsensical assignments will throw.
     */
    void fromParseNode(ParseNode& node);
    
  protected:
    
    /**
     * The object's type name.
     */
    std::string type_;
    
    /**
     * The object's value as an int. Only applicable to integer-like types.
     */
    DiscasterInt intValue_;
    
    /**
     * The object's value as a string. Only applicable to string-like types.
     */
    DiscasterString stringValue_;
    
    /**
     * The object's value as an array. Only applicable to arrays.
     */
    ObjectArray arrayValue_;
    
    /**
     * The object's value as a function (pointer). Only applicable to functions.
     */
    ScriptFunctionPointer functionPointerValue_;
    
    /**
     * The object's members. Only applicable to "blob" types (those not
     * represented by any of the above).
     */
    ObjectMemberMap members_;
    
    /**
     * Helper for adding function pointers to constructed objects.
     */
    void addFunction(std::string name, ScriptFunctionPointer funcP);
    
  };
  
  /**
   * Object "subtypes".
   * IMPORTANT: When stored as members of an Object, these are Objects, not
   *            pointers.
   *            In other words, they will be "sliced" to Objects on assignment.
   *            DO NOT EXTEND SUBCLASSES WITH ADDITIONAL MEMBERS OR
   *            FUNCTIONALITY. THEY WILL BE LOST.
   *            Subclasses should provide constructor(s) and nothing else.
   */
  
  class IntObject : public Object {
  public:
    IntObject();
    IntObject(DiscasterInt value);
  };
  
  class StringObject : public Object {
  public:
    StringObject();
    StringObject(const DiscasterString& value);
  };
  
  class ArrayObject : public Object {
  public:
    ArrayObject();
    ArrayObject(const ObjectArray& value);
  };
  
  class FunctionPointerObject : public Object {
  public:
    FunctionPointerObject();
    FunctionPointerObject(ScriptFunctionPointer value);
  };
  
  class BlobObject : public Object {
  public:
    BlobObject();
    BlobObject(const std::string& typeName);
  };


}


#endif
