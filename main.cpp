#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "json_any.h"

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
  void fromJson(json::JsonValue& d) {
    if (d.HasMember("type") && d["type"].IsString()) {
      type = d["type"].GetString();
    }
    if (d.HasMember("age") && d["age"].IsInt()) {
      age = d["age"].GetInt();
    }
  }
  bool operator==(const Singer& rh) {
    if (type == rh.type && age == rh.age) {
      return true;
    }
    return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const Singer& dt) {
    os << "Singer : [" << std::endl;
    os << "type:" << dt.type << std::endl;
    os << "age:" << dt.age << std::endl;
    os << "]" << std::endl;
    return os;
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
  void fromJson(json::JsonValue& d) {
    if (d.HasMember("country") && d["country"].IsString()) {
      country = d["country"].GetString();
    }
    if (d.HasMember("city") && d["city"].IsString()) {
      city = d["city"].GetString();
    }
    if (d.HasMember("street") && d["street"].IsString()) {
      street = d["street"].GetString();
    }
  }
  bool operator==(const Address& rh) {
    if (country == rh.country && city == rh.city && street == rh.street) {
      return true;
    }
    return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const Address& dt) {
    os << "Address : [" << std::endl;
    os << "country:" << dt.country << std::endl;
    os << "city:" << dt.city << std::endl;
    os << "street:" << dt.street << std::endl;
    os << "]" << std::endl;
    return os;
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

  void fromJson(json::JsonValue& d) {
    if (d.HasMember("relation") && d["relation"].IsString()) {
      relation = d["relation"].GetString();
    }
    if (d.HasMember("secret")) {
      secret.fromJson(d["secret"]);
    }
  }

  bool operator==(const Friend& rh) {
    if (relation == rh.relation && secret == rh.secret) {
      return true;
    }
    return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const Friend& dt) {
    os << "Friend : [" << std::endl;
    os << "relation:" << dt.relation << std::endl;
    os << "secret:" << dt.secret << std::endl;
    os << "]" << std::endl;
    return os;
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
    for (auto v : _friends) {
      v.toJson(writer);
    }
    writer.EndArray();
    writer.Key("secret");
    secret.toJson(writer);
    writer.EndObject();
  }

  void fromJson(json::JsonValue& d) {
    if (d.HasMember("name") && d["name"].IsString()) {
      name = d["name"].GetString();
    }
    if (d.HasMember("age") && d["age"].IsInt()) {
      age = d["age"].GetInt();
    }
    if (d.HasMember("address")) {
      address.fromJson(d["address"]);
    }
    if (d.HasMember("_friends") && d["_friends"].IsArray()) {
      // address.fromJson(d["_friends"]);
      auto friends = d["_friends"].GetArray();
      for (int i = 0; i < friends.Size(); i++) {
        _friends[i].fromJson(friends[i]);
        //_friends.push_back(f);
      }
    }
    if (d.HasMember("secret")) {
      secret.fromJson(d["secret"]);
    }
  }

  bool operator==(const Person& rh) {
    if (name == rh.name && age == rh.age && address == rh.address &&
        secret == rh.secret) {
      if (_friends.size() != rh._friends.size()) {
        return false;
      }
      for (int i = 0; i < _friends.size(); i++) {
        if (_friends[i] == rh._friends[i]) {
          continue;
        } else {
          return false;
        }
      }
      return true;
    }
    return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const Person& dt) {
    os << "Person : [" << std::endl;
    os << "name:" << dt.name << std::endl;
    os << "age:" << dt.age << std::endl;
    os << "address:" << dt.address << std::endl;
    os << "_friends:" << std::endl;
    for(auto f:dt._friends){
        os << f << std::endl;
    }
    os << "secret:" << dt.secret << std::endl;

    os << "]" << std::endl;
    return os;
  }
};

int main() {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  Friend f1{"my best friend", Singer{"rocker", 18}};
  Friend f2{"new friend", "little girl"};
  Friend f3{"third friend", 3};
  // Friend f4{"hello", std::vector<std::string>{"a", "b"}};

  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  Address addr1{"china", "beijing", "wangjing"};
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};

  json::any f22 = p1;
  f22.toJson(writer);
  std::string json = buffer.GetString();
  std::cout << json << std::endl;

  rapidjson::Document document;
  document.Parse(json.c_str());

  Friend f11{"", Singer{"", 18}};
  Friend f21{"", std::string("")};
  Friend f31{"", 3};

  Person p3;
  p3.secret = std::string();
  //对于新生成的对象，any并不知道其真实的数据类型是什么，所以需要手工指定。
  p3._friends.push_back(f11);
  p3._friends.push_back(f21);
  p3._friends.push_back(f31);

  p3.fromJson(document);
  std::cout << "p3 is equal p1:" << (p3 == p1) << std::endl;
  std::cout << p3;
  return 0;
}