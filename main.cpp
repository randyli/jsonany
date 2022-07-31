#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include  "boost_any.h"

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

  void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer_) {
    writer_.StartObject();
    writer_.Key("a");
    writer_.Int(a);
    writer_.Key("b");
    writer_.Double(b);
    writer_.EndObject();
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
    void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer_) {
    writer_.StartObject();
    writer_.Key("type");
    writer_.String(type.c_str());
    writer_.Key("age");
    writer_.Int(age);
    writer_.EndObject();
  }
};
struct Address {
    std::string country;
    std::string city;
    std::string street;
    std::vector<Person> neighbors;
};
struct Friend {
    std::string relation;
    json::any secret;
    void toJson(rapidjson::Writer<rapidjson::StringBuffer>& writer_) {
    writer_.StartObject();
    writer_.Key("relation");
    writer_.String(relation.c_str());
    writer_.Key("secret");
    secret.toJson(writer_);
    writer_.EndObject();
    }

};
struct Person {
    std::string name;
    int age;
    Address address;
    std::vector<Friend> _friends;
    json::any secret;
};


int main() {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer_(buffer);

  Friend f1{"my best friend", Singer{"rocker", 18}};
  Friend f2{"new friend", "little girl"};
  Friend f3{"third friend", 3};
  //Friend f4{"hello", std::vector<std::string>{"a", "b"}};
  json::any f22 = f3;
  f22.toJson(writer_);
  std::cout << buffer.GetString();
  return 0;
}