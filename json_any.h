#pragma once

#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace json {

class any;

using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;
using JsonDoc = rapidjson::Document;
using JsonValue = rapidjson::Value;
struct GetterSetter {
  std::function<void(JsonWriter&)> getter;
  std::function<void(JsonDoc&)> setter;
};

template<typename T>
void writeToJson(T v, JsonWriter& writer) {
  v.toJson(writer);
}

template<>
void writeToJson(int v, JsonWriter& writer){
  writer.Int(v);
}
template<>
void writeToJson(float v, JsonWriter& writer){
  writer.Double(v);
}
template<>
void writeToJson(std::string v, JsonWriter& writer){
  writer.String(v.c_str());
}
template<>
void writeToJson(const char* v, JsonWriter& writer){
  writer.String(v);
}
template <class T>
void writeToJson(std::vector<T>& v, JsonWriter& writer){
  writer.StartArray();
  for(auto it : v){
    writeToJson(it, writer);
  }
  writer.EndArray();
}

template<typename T>
void readFromJson(T& v, JsonValue& jv) {
  v.fromJson(jv);
}

template<>
void readFromJson(int& v, JsonValue& jv){
  v = jv.GetInt();
}
template<>
void readFromJson(float& v, JsonValue& jv){
  v = jv.GetDouble();
}

template<>
void readFromJson(const char* &v, JsonValue& jv){
  
}

template<>
void readFromJson(std::string& v, JsonValue& jv){
  v = jv.GetString();
}

template <class T>
void readFromJson(std::vector<T>& v, JsonValue& jv){
  if(!jv.IsArray()) {
    return;
  }
  //T obj;
  auto jarray = jv.GetArray();
  for (int i = 0; i < jarray.Size(); i++) {
    readFromJson(v[i], jarray[i]);
    //v[i]push_back(obj);
  }
}

using JsonReflector = std::map<std::string, GetterSetter>;

template <class Type>
Type any_cast(any&);
template <class Type>
Type any_cast(const any&);
template <class Type>
Type* any_cast(any*);
template <class Type>
const Type* any_cast(const any*);

struct bad_any_cast : public std::bad_cast {};

namespace {
struct placeholder {
  virtual std::unique_ptr<placeholder> clone() const = 0;
  virtual const std::type_info& type() const = 0;
  virtual void toJson(JsonWriter& writer) = 0;
  virtual void fromJson(JsonValue& value) = 0;
  virtual ~placeholder() {}
};
template <class T>
struct concrete : public placeholder {
  concrete(T&& x) : value(std::move(x)) {}
  concrete(const T& x) : value(x) {}
  virtual std::unique_ptr<placeholder> clone() const override {
    return std::unique_ptr<placeholder>(new concrete<T>(value));
  }
  virtual const std::type_info& type() const override { return typeid(T); }
  virtual void toJson(JsonWriter& writer) override {
    //value.toJson(writer);
    writeToJson(value, writer);
  }

  virtual void fromJson(JsonValue& jv) override {
    //value.toJson(writer);
    readFromJson(value,jv);
  }

  T value;
};
}

class any {
 public:
  template <class Type>
  friend Type any_cast(any&);

  template <class Type>
  friend Type any_cast(const any&);

  template <class Type>
  friend Type* any_cast(any*);

  template <class Type>
  friend const Type* any_cast(const any*);

  any() : ptr(nullptr) {}

  any(any&& x) : ptr(std::move(x.ptr)) {}

  any(const any& x) {
    if (x.ptr) ptr = x.ptr->clone();
  }

  template <class Type>
  any(const Type& x)
      : ptr(new concrete<typename std::decay<const Type>::type>(x)) {}

  any& operator=(any&& rhs) {
    ptr = std::move(rhs.ptr);
    return (*this);
  }

  any& operator=(const any& rhs) {
    ptr = std::move(any(rhs).ptr);
    return (*this);
  }

  template <class T>
  any& operator=(T&& x) {
    ptr.reset(new concrete<typename std::decay<T>::type>(
        typename std::decay<T>::type(x)));
    return (*this);
  }

  template <class T>
  any& operator=(const T& x) {
    ptr.reset(new concrete<typename std::decay<T>::type>(
        typename std::decay<T>::type(x)));
    return (*this);
  }

  void clear() { ptr.reset(nullptr); }

  bool empty() const { return ptr == nullptr; }

  const std::type_info& type() const {
    return (!empty()) ? ptr->type() : typeid(void);
  }

  void toJson(JsonWriter& writer) {  
    ptr->toJson(writer); 
  }

  void fromJson(JsonValue& value) {  
    ptr->fromJson(value);
  }

 private:
  std::unique_ptr<placeholder> ptr;
};

template <class Type>
Type any_cast(any& val) {
  if (val.ptr->type() != typeid(Type)) throw bad_any_cast();
  return static_cast<concrete<Type>*>(val.ptr.get())->value;
}

template <class Type>
Type any_cast(const any& val) {
  return any_cast<Type>(any(val));
}

template <class Type>
Type* any_cast(any* ptr) {
  return dynamic_cast<Type*>(ptr->ptr.get());
}

template <class Type>
const Type* any_cast(const any* ptr) {
  return dynamic_cast<const Type*>(ptr->ptr.get());
}
}  // namespace json