#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <map>

#include "json_any.h"

struct  MyInner{
  int a;
  float b;

  json::JsonReflector __reflector__ = {
    {
        "a", 
        {
            [=](json::JsonWriter &writer)->void{},
            [=](json::JsonDoc &doc)->void{}
        }
    }
  };

  void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
    writer.StartObject();
    writer.Key("a");
    writer.Int(a);
    writer.Key("b");
    writer.Double(b);
    writer.EndObject();
  }

  void fromJson(rapidjson::Document& d) {
    if(d.HasMember("a") && d["a"].IsInt()) {
      a = d["a"].GetInt();
    }
    if(d.HasMember("b") && d["b"].IsDouble()) {
      b = d["b"].GetDouble();
    }
  }
};

struct Person;
struct Singer {
    std::string type;
    int age;
    void toJson(json::JsonWriter& writer) {
    writer.StartObject();
    writer.Key("type");
    writer.String(type.c_str());
    writer.Key("age");
    writer.Int(age);
    writer.EndObject();
  }
};
struct Address {
    std::string country;
    std::string city;
    std::string street;

    void toJson(json::JsonWriter& writer) { 
      writer.StartObject();
      writer.Key("country");
      writer.String(country.c_str());
      writer.Key("city");
      writer.String(city.c_str());
      writer.Key("street");
      writer.String(street.c_str());
      writer.EndObject();
    }
};
struct Friend {
    std::string relation;
    json::any secret;
    void toJson(json::JsonWriter& writer) {
    writer.StartObject();
    writer.Key("relation");
    writer.String(relation.c_str());
    writer.Key("secret");
    secret.toJson(writer);
    writer.EndObject();
    }

};
struct Person {
    std::string name;
    int age;
    Address address;
    std::vector<Friend> _friends;
    json::any secret;

    void toJson(json::JsonWriter& writer) { 
        writer.StartObject();
        writer.Key("name");
        writer.String(name.c_str());
        writer.Key("age");
        writer.Int(age);
        writer.Key("address");
        address.toJson(writer);
        writer.Key("_friends");
        writer.StartArray();
        for(auto v : _friends) {
          v.toJson(writer);
        }
        writer.EndArray();
        writer.Key("secret");
        secret.toJson(writer);
        writer.EndObject();
    }
};


int main() {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  Friend f1{"my best friend", Singer{"rocker", 18}};
  Friend f2{"new friend", "little girl"};
  Friend f3{"third friend", 3};
  //Friend f4{"hello", std::vector<std::string>{"a", "b"}};
  
  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  Address addr1{"china", "beijing", "wangjing"};
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};
  
  json::any f22 = p1;
  f22.toJson(writer);
  std::cout << buffer.GetString();
  return 0;
}