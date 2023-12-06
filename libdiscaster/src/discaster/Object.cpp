#include "discaster/Object.h"
#include "exception/TGenericException.h"

namespace Discaster {
  
  
  std::string Object::type() const { return type_; }
  void Object::setType(const std::string& type__)
    { type_ = type__; }
  
  DiscasterInt Object::intValue() const { return intValue_; }
  void Object::setIntValue(DiscasterInt intValue__)
    { intValue_ = intValue__; }
  
  const DiscasterString& Object::stringValue() const { return stringValue_; }
  void Object::setStringValue(const DiscasterString& stringValue__)
    { stringValue_ = stringValue__; }
  
  const ObjectArray& Object::arrayValue() const { return arrayValue_; }
  ObjectArray& Object::arrayValue() { return arrayValue_; }
  void Object::setArrayValue(const ObjectArray& arrayValue__)
    { arrayValue_ = arrayValue__; }
  
  const ScriptFunctionPointer& Object::functionPointerValue() const
    { return functionPointerValue_; }
  void Object::setFunctionPointerValue(
    const ScriptFunctionPointer& functionPointerValue__)
    { functionPointerValue_ = functionPointerValue__; }
    
  bool Object::hasMember(const DiscasterString& name) const {
    ObjectMemberMap::const_iterator it = members_.find(name);
    if (it != members_.cend()) return true;
    return false;
  }
  
  Object& Object::getMember(const DiscasterString& name) {
    ObjectMemberMap::iterator it = members_.find(name);
    if (it == members_.end()) {
      throw BlackT::TGenericException(
        T_SRCANDLINE, "Object::member()",
        std::string("Accessed non-existent member: ")
          + name);
    }
    
    return it->second;
  }
  
  const Object& Object::getMember(const DiscasterString& name) const {
    ObjectMemberMap::const_iterator it = members_.find(name);
    if (it == members_.cend()) {
      throw BlackT::TGenericException(
        T_SRCANDLINE, "Object::member() const",
        std::string("Accessed non-existent member: ")
          + name);
    }
    
    return it->second;
  }
  
  void Object::setMember(const DiscasterString& name, const Object& obj) {
    members_[name] = obj;
  }
  
  void Object::setMember(const DiscasterString& name, DiscasterInt intValue__) {
    members_[name] = IntObject(intValue__);
  }
  
  void Object::setMember(
      const DiscasterString& name, const DiscasterString& stringValue__) {
    members_[name] = StringObject(stringValue__);
  }
  
  DiscasterInt Object::getMemberInt(const DiscasterString& name) const {
    return getMember(name).intValue();
  }
  
  DiscasterString Object::getMemberString(const DiscasterString& name) const {
    return getMember(name).stringValue();
  }
  
  const ObjectMemberMap& Object::members() const {
    return members_;
  }
  
  ObjectMemberMap& Object::members() {
    return members_;
  }
    
  void Object::fromParseNode(ParseNode& node) {
    if (node.isType("INTEGER")) {
      *this = IntObject(node.intValue());
    }
    else if (node.isType("STRING")) {
      *this = StringObject(node.stringValue());
    }
    // FIXME:
    // we shouldn't use direct pointers for this.
    // objects are in maps and memory will be erratically reassigned.
    // or does it matter?
    else if (node.isType("OBJECTP")) {
      *this = *(Object*)node.voidp();
    }
    else {
      throw BlackT::TGenericException(
        T_SRCANDLINE, "Object::fromParseNode() const",
        std::string("Assigned from invalid type: ")
          + node.type());
    }
  }
  
  void Object::addFunction(std::string name, ScriptFunctionPointer funcP) {
    setMember(name, FunctionPointerObject(funcP));
  }
  
  IntObject::IntObject() {
    type_ = "int";
    intValue_ = 0;
  }
  
  IntObject::IntObject(DiscasterInt value) {
    type_ = "int";
    intValue_ = value;
  }
  
  StringObject::StringObject() {
    type_ = "string";
  }
  
  StringObject::StringObject(const DiscasterString& value) {
    type_ = "string";
    stringValue_ = value;
  }
  
  ArrayObject::ArrayObject() {
    type_ = "array";
  }
  
  ArrayObject::ArrayObject(const ObjectArray& value) {
    type_ = "array";
    arrayValue_ = value;
  }
  
  FunctionPointerObject::FunctionPointerObject() {
    type_ = "functionpointer";
  }
  
  FunctionPointerObject::FunctionPointerObject(
      ScriptFunctionPointer value) {
    type_ = "functionpointer";
    functionPointerValue_ = value;
  }
  
  BlobObject::BlobObject() {
    type_ = "blob";
  }
  
  BlobObject::BlobObject(
      const std::string& name) {
//    type_ = "blob";
//    stringValue_ = name;
    
    type_ = name;
  }
  

}
