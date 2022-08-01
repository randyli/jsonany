#pragma once

#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace json {

class any;

using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;
using JsonDoc = rapidjson::Document;
using JsonValue = rapidjson::Value;

// 不同数据类型输出json的模板函数
// 对于基本数据类型，int float string等直接调用rapidjson的对应函数
// 对于其他类型调用其toJson函数
// 每个基本类型有对应的特化
template <typename T>
void writeToJson(T v, JsonWriter& writer) {
  v.toJson(writer);
}
template <>
void writeToJson(int v, JsonWriter& writer) {
  writer.Int(v);
}
template <>
void writeToJson(float v, JsonWriter& writer) {
  writer.Double(v);
}
template <>
void writeToJson(std::string v, JsonWriter& writer) {
  writer.String(v.c_str());
}
template <>
void writeToJson(const char* v, JsonWriter& writer) {
  writer.String(v);
}
template <class T>
void writeToJson(std::vector<T>& v, JsonWriter& writer) {
  writer.StartArray();
  for (auto it : v) {
    writeToJson(it, writer);
  }
  writer.EndArray();
}

// 不同数据类型读取json值的模板函数
// 基本原理同上，基本类型特化，自定义类型调用其fromJson函数
template <typename T>
void readFromJson(T& v, JsonValue& jv) {
  v.fromJson(jv);
}

template <>
void readFromJson(int& v, JsonValue& jv) {
  v = jv.GetInt();
}
template <>
void readFromJson(float& v, JsonValue& jv) {
  v = jv.GetDouble();
}

template <>
void readFromJson(const char*& v, JsonValue& jv) {}

template <>
void readFromJson(std::string& v, JsonValue& jv) {
  v = jv.GetString();
}

template <class T>
void readFromJson(std::vector<T>& v, JsonValue& jv) {
  if (!jv.IsArray()) {
    return;
  }
  // T obj;
  auto jarray = jv.GetArray();
  for (int i = 0; i < jarray.Size(); i++) {
    readFromJson(v[i], jarray[i]);
    // v[i]push_back(obj);
  }
}
// 比较模版函数
// 基本原理同上，基本类型特化
template <typename T>
bool isEqual(T& l, T& r) {
  return l == r;
}

template <>
bool isEqual(int& l, int& r) {
  return l == r;
}
template <>
bool isEqual(float& l, float& r) {
  return std::abs(l - r) < 0.001;
}

template <>
bool isEqual(const char*& l, const char*& r) {
  return std::strcmp(l, r) == 0;
}

template <>
bool isEqual(std::string& l, std::string& r) {
  return l == r;
}

template <class T>
bool isEqual(std::vector<T>& l, std::vector<T>& r) {
  if (l.size() != r.size()) {
    return false;
  }
  for (int i = 0; i < l.size(); i++) {
    if (l == r) {
      continue;
    }
    return false;
  }
  return true;
}
// 输出模板函数
// 基本原理同上，基本类型特化
template <typename T>
void printOut(T v, std::ostream& os) {
  os << v;
}

template <>
void printOut(int v, std::ostream& os) {
  os << v;
}
template <>
void printOut(float v, std::ostream& os) {
  os << v;
}
template <>
void printOut(std::string v, std::ostream& os) {
  os << v;
}
template <>
void printOut(const char* v, std::ostream& os) {
  os << v;
}
template <class T>
void printOut(std::vector<T>& v, std::ostream& os) {
  for (auto it : v) {
    printOut(it, os);
  }
}

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
//保存数据的基本结构
struct placeholder {
  virtual std::unique_ptr<placeholder> clone() const = 0;
  virtual const std::type_info& type() const = 0;
  virtual void toJson(JsonWriter& writer) = 0;
  virtual void fromJson(JsonValue& value) = 0;
  virtual bool equal(const placeholder* rh) = 0;
  virtual void print(std::ostream& os) = 0;
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
    // value.toJson(writer);
    writeToJson(value, writer);
  }

  virtual void fromJson(JsonValue& jv) override {
    // value.toJson(writer);
    readFromJson(value, jv);
  }

  virtual bool equal(const placeholder* rh) override {
    auto p = (concrete<T>*)rh;
    return isEqual(value, p->value);
  }

  virtual void print(std::ostream& os) override { printOut(value, os); };

  T value;
};

//对cons char *的特化，对于比较和赋值等操作，直接用const char* 显然会出错。
template <>
struct concrete<const char*> : public placeholder {
  concrete(const char* x) : value(x) {}
  virtual std::unique_ptr<placeholder> clone() const override {
    return std::unique_ptr<placeholder>(new concrete<std::string>(value));
  }
  virtual const std::type_info& type() const override {
    return typeid(std::string);
  }
  virtual void toJson(JsonWriter& writer) override {
    // value.toJson(writer);
    writeToJson(value, writer);
  }

  virtual void fromJson(JsonValue& jv) override {
    // value.toJson(writer);
    readFromJson(value, jv);
  }

  virtual bool equal(const placeholder* rh) override {
    auto p = (concrete<std::string>*)rh;
    return value == p->value;
  }
  virtual void print(std::ostream& os) override { os << value; }
  std::string value;
};

}  // namespace

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

  void toJson(JsonWriter& writer) { ptr->toJson(writer); }

  void fromJson(JsonValue& value) { ptr->fromJson(value); }

  bool operator==(const any& rh) { return ptr->equal(rh.ptr.get()); }

  friend std::ostream& operator<<(std::ostream& os, const any& dt) {
    dt.ptr->print(os);
    return os;
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

}  // namespace json