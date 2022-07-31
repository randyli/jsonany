#include <any>
#include <iostream>
#include <string>
#include <vector>

#include "json_encoder.h"

#include "json_any.h"

struct MyInner : public json::JsonSeriable{
  int a;
  std::vector<json::Property> properties() override {
    std::vector<json::Property> ps;
    ps.push_back({"a", a});
    return ps;
  }
  json::JsonSeriable* clone() override { return nullptr; }
};

struct MyInt : public json::JsonSeriable{
  my::any sec;
  std::vector<json::Property> properties() override {
    std::vector<json::Property> ps;
    ps.push_back({"sec", sec});
    return ps;
  }
};
union A {
  std::string a;
};
int main() {
  MyInner inner;
  inner.a = 2;
  json::any c = 3;
  json::any d = inner;
  
  json::JsonSeriable* j = dynamic_cast<json::JsonSeriable*>(&inner);
  if(j == nullptr) {
    std::cout << "error" << std::endl;
  }
  MyInt m;
  m.sec = inner;
  json::JsonEncoder encoder;
  try{
    std::cout << encoder.Dump(&m) << std::endl;
  }catch(my::bad_any_cast e) {
    std::cerr << e.what();
  }

  return 0;
}