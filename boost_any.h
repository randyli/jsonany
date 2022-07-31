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
struct GetterSetter {
  std::function<void(JsonWriter&)> getter;
  std::function<void(JsonDoc&)> setter;
};

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

struct placeholder {
  virtual std::unique_ptr<placeholder> clone() const = 0;
  virtual const std::type_info& type() const = 0;
  virtual void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) = 0;
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
  virtual void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) override {
    value.toJson(writer);
  }
  T value;
};

template <>
struct concrete<std::string> : public placeholder {
  concrete(std::string&& x) : value(std::move(x)) {}
  concrete(const std::string& x) : value(x) {}
  virtual std::unique_ptr<placeholder> clone() const override {
    return std::unique_ptr<placeholder>(new concrete<std::string>(value));
  }
  virtual const std::type_info& type() const override { return typeid(std::string); }
  virtual void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) override {
    //value.toJson(writer);
    writer.String(value.c_str());
  }
  std::string value;
};

template <>
struct concrete<int> : public placeholder {
  concrete(int&& x) : value(x) {}
  concrete(const int& x) : value(x) {}
  virtual std::unique_ptr<placeholder> clone() const override {
    return std::unique_ptr<placeholder>(new concrete<int>(value));
  }
  virtual const std::type_info& type() const override { return typeid(int); }
  virtual void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) override {
    writer.Int(value);
  }
  int value;
};
template <>
struct concrete<float> : public placeholder {
  concrete(float&& x) : value(x) {}
  concrete(const float& x) : value(x) {}
  virtual std::unique_ptr<placeholder> clone() const override {
    return std::unique_ptr<placeholder>(new concrete<float>(value));
  }
  virtual const std::type_info& type() const override { return typeid(float); }
  virtual void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) override {
    writer.Double(value);
  }
  float value;
};


template <>
struct concrete<const char*> : public placeholder {
  concrete(const char* x) : value(x) {}
  //concrete(std::string& x) : value(x) {}
  virtual std::unique_ptr<placeholder> clone() const override {
    return std::unique_ptr<placeholder>(new concrete<const char*> (value));
  }
  virtual const std::type_info& type() const override { return typeid(const char*); }
  virtual void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) override {
    writer.String(value);
  }
  const char* value;
};


template <class T>
struct concrete<std::vector<T>> : public placeholder {
  concrete(std::vector<T>&& x) : value(std::move(x)) {}
  concrete(const std::vector<T>& x) : value(x) {}
  virtual std::unique_ptr<placeholder> clone() const override {
    return std::unique_ptr<placeholder>(new concrete<std::vector<T>>(value));
  }
  virtual const std::type_info& type() const override { return typeid(std::vector<T>); }
  virtual void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) override {
    
  }
  std::vector<T> value;
};

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

  void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) {  
    ptr->toJson(writer); 
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