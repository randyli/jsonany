#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "boost_any.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace json {
struct Property {
  std::string name;
  my::any value;
};

class JsonSeriable {
public:
  virtual std::vector<Property> properties() = 0;
  virtual JsonSeriable* clone() = 0;
};

class JsonEncoder {
public:
  JsonEncoder(): writer_(buffer_){
    
  }
  std::string Dump(JsonSeriable* obj);

private:
 void dump(JsonSeriable* obj);
 void dump(my::any v);

private:
  JsonSeriable* obj_;
  rapidjson::StringBuffer buffer_;
  rapidjson::Writer<rapidjson::StringBuffer> writer_;
};

std::string JsonEncoder::Dump(JsonSeriable* obj) {
  dump(obj);
  return buffer_.GetString();
}

void JsonEncoder::dump(JsonSeriable* obj) {
    writer_.StartObject();
    auto properties = obj->properties();
    for(auto p : properties) {
        writer_.Key(p.name.c_str());
        dump(p.value);
    }
    writer_.EndObject();
}

void JsonEncoder::dump(my::any v) {
  if(v.empty()) {
    writer_.Null();
  }else if(v.type() == typeid(int)){
    writer_.Int(my::any_cast<int>(v));
  }else if(v.type() == typeid(uint32_t)){
    writer_.Uint(my::any_cast<uint32_t>(v));
  }else if(v.type() == typeid(float)) {
    writer_.Double(my::any_cast<float>(v));
  }else if(v.type() == typeid(int64_t)) {
    writer_.Int64(my::any_cast<int64_t>(v));
  }else if(v.type() == typeid(uint64_t)) {
    writer_.Uint64(my::any_cast<uint64_t>(v));
  }else if(v.type() == typeid(bool)) {
    writer_.Bool(my::any_cast<bool>(v));
  }else if(v.type() == typeid(std::string)) {
    writer_.String(my::any_cast<std::string>(v).c_str());
  }else {
    auto j = my::any_cast<JsonSeriable>(&v);
    if(j == nullptr) {
      std::cout << "bad cast";
      return;
    }
    dump(j);
  }
}

}