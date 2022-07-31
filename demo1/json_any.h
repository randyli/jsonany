#pragma once

#include <exception>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <string>
namespace json {

class any;
class JsonSeriable;
struct bad_any_cast : public std::bad_cast {};

enum class JsonTypes { 
    kInt,
    kFloat,
    kString,
    kObject
};

struct AnyData {
  JsonTypes type;
  union PlaceHolder{
    int intVal;
    float floatVal;
    JsonSeriable* obj;
  } data;
};


class any {
 public:
  any() : ptr(nullptr) {}

  any(any&& x) : ptr(std::move(x.ptr)) {}

  //clone
  any(const any& x) {
    
  }
   
  any(JsonSeriable& x) {
    auto p = new AnyData;
    p->type = JsonTypes::kObject;
    //p->data.obj = x->clone();
  }

  any(const int v) {
    auto p = new AnyData;
    p->type = JsonTypes::kInt;
    p->data.intVal = v;
    ptr.reset(p);
  }

 private:
  std::unique_ptr<AnyData> ptr;
};
}  // namespace json