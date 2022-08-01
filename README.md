# json any 

# 可参考的项目

### boost any的实现

### json序列化的一些参考
https://uscilab.github.io/cereal/index.html

https://stackoverflow.com/questions/17549906/c-json-serialization

https://github.com/jgaa/restc-cpp

protobuf

### any的实现原理

1 封装了 void* 

2 通过对构造函数的模版化，实现编译期多态

3 std::any 通过判断变量大小来决定是否在堆上分配空间还是栈上分配空间，堆上分配使用带参数的new

### json序列化

1 struct序列化与反序列化需要提供反射机制。c++中比较困难，基本上都需要侵入代码。

### 已经完成的功能

1 any功能

2 json序列化，需要结构中提供toJson方法，后续准备用宏替换，以简化代码。

### 待完成的功能

1 vector json序列化 -- OK

2 嵌套序列化  -- OK

3 反序列化 -- OK

4 简化any代码

5 单元测试