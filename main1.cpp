#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <any>
//#include "json_any.h"
#include "json_encoder.h"


struct GetterSetter {
  std::function<std::any()> getter;
  std::function<void(std::any)> setter;
};

#define REFLECT_BEGIN(CLASS)  friend std::ostream& operator<<(std::ostream& os, const CLASS& inner) { \
    for (auto it : inner.__gettersetter__) { \
      os << it.first << ":"; \
      auto getter = it.second.getter; \
      std::any val = getter(); \
      if(val.type() == typeid(int)) { \
        os << std::any_cast<int>(val) << std::endl; \
      }else if(val.type() == typeid(float)){ \
        os << std::any_cast<float>(val) << std::endl; \
      } \
    } \
    return os; \
  } \
  std::map<std::string, GetterSetter> __gettersetter__ = { 

#define REFLECT_ADD(TYPE, NAME)  {#NAME, { \
      [=]() -> std::any { return NAME; }, \
      [=](std::any a_) {  NAME = std::any_cast<TYPE>(a_); } \
      } \
    }

#define REFLECT_END }

struct Person {
  int a;
  int b;
  REFLECT_BEGIN(Person)
  REFLECT_ADD(int, a), 
  REFLECT_ADD(int, b) 
  REFLECT_END;
};

struct MyInner {
  int a;
  float b;
  Person p;
  REFLECT_BEGIN(MyInner)
  REFLECT_ADD(int, a), 
  REFLECT_ADD(float, b), 
  REFLECT_ADD(Person, p) 
  REFLECT_END;
  /*
  friend std::ostream& operator<<(std::ostream& os, const MyInner& inner) {
    for (auto it : inner.__gettersetter__) {
      os << it.first << ":";
      auto getter = it.second.getter;
      std::any val = getter();
      if(val.type() == typeid(int)) {
        os << std::any_cast<int>(val) << std::endl;
      }else if(val.type() == typeid(float)){
        os << std::any_cast<float>(val) << std::endl;
      }
    }
    return os;
  }
*/
  /*
    std::map<std::string, GetterSetter> __gettersetter__ = {
      {"a", {
        [=]() -> std::any { return a; },
        [=](std::any a_) {  a = std::any_cast<int>(a_); }
        }
      },
      {"b",{
        [=]() -> std::any { return b; },
        [=](std::any b_) {  b = std::any_cast<float>(b_); }
      }
      }
    };
    */
};

int main() {
  MyInner inner {1, 2.1};
  std::cout << inner;
  json::JsonEncoder encoder;
  std::string json = encoder.Dump(inner);
  std::cout << json << std::endl;

  MyInner inner2;
  json::JsonDecoder decoder(json);
  decoder.ParseTo(inner2);

  std::cout << inner.a << " " << inner2.a << std::endl;
  return 0;
}