#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace json {

class JsonEncoder {
public:
  JsonEncoder(): writer_(buffer_){
    
  }
  template<typename T>
  std::string Dump(T& obj);

  void dump(std::any obj){
    
  }

 private:
  rapidjson::StringBuffer buffer_;
  rapidjson::Writer<rapidjson::StringBuffer> writer_;
};


template<typename T>
std::string JsonEncoder::Dump(T& obj) {
  writer_.StartObject();
  for (auto it : obj.__gettersetter__) {
    writer_.Key(it.first.c_str());
    std::function<std::any()> getter = it.second.getter;
    std::any v = getter();
    //writer_.Int();
    if(v.type() == typeid(int)){
      writer_.Int(std::any_cast<int>(v));
    }else if(v.type() == typeid(float)) {
      writer_.Double(std::any_cast<float>(v));
    }else {
      //writer_.StartObject();
      Dump(v);
      //writer_.EndObject();
    }
  }
  writer_.EndObject();
  return buffer_.GetString();
}

class JsonDecoder {
public:
  JsonDecoder(std::string& json) {
    doc.Parse(json.c_str());  
  }
  template<typename T>
  void ParseTo(T& obj) {
    for (rapidjson::Value::ConstMemberIterator it = doc.MemberBegin();
    it != doc.MemberEnd(); ++it){
      std::string fieldName = it->name.GetString();
      std::function<void (std::any)> setter = obj.__gettersetter__[fieldName].setter;
      if(it->value.IsInt()){
        setter(it->value.GetInt());
      }else if(it->value.IsFloat()){
        setter(it->value.GetFloat());
      }
      
    }
  }
 private:
   rapidjson::Document doc;

};
}