#pragma once

#include <exception>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <string>
namespace json {

struct bad_any_cast : public std::bad_cast {};

enum class JsonTypes { kNull, kInt, kFloat, kString, kObject };

union PlaceHolder{
  int intVal;
  float floatVal;
};

class Any;

struct GetterSetter {
  std::function<Any()> getter;
  std::function<void(Any&)> setter;
};

using Reflector = std::map<std::string, GetterSetter>;

class Any {
public:
 /*
 Any(int v) { 
   type_ = JsonTypes::kInt;
   placeholder_.intVal = v;
 }
 Any(float v) { 
   type_ = JsonTypes::kFloat;
   placeholder_.floatVal = v;
 }
 */
 template<typename T>
 Any (T &v) : type(std::type_index(typeid(T))){
   T* t = new T;
   *t = v;
   placeholder = t;
   //type = std::type_index(typeid(T));
   
   reflector = v.__reflector__;
 }

 template<typename T> 
 T cast() {
   return dynamic_cast<T>(*placeholder);
 }

 std::string toJson() { 
  
  return "";
 }

 void fromJson(std::string json) {

 }



 Any& operator=(Any& any) { 
   this->placeholder = any.placeholder;
   return *this;
 }


private:
  void* placeholder;
  std::type_index type;
  std::function<bool(Any)> equal;
  //std::function<std::string()> toJson;
  Reflector reflector;
};

}  // namespace json