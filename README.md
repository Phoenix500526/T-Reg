# T-Reg
## 项目描述
T-Reg 是一个学习型项目，利用 Follow Set，DFA 构造了一个简易的正则表达式引擎，目前支持 () [^] | . * ? + 以及匹配串联等功能。本项目采用 cmake 构建，clang 编译，并由 conan 进行包管理(主要管理 gtest 包)

## 环境配置
* Ubuntu 16.04.2 LTS
* C++ 11
* clang version 3.8.0-2ubuntu4
* conan

一些相关的安装命令如下：
```shell
# 安装 conan
$ sudo pip install conan
# 安装 clang, mysql 相关
$ sudo apt-get intall clang-3.8 
```

## 编译方法
```shell
# 默认编译方式采用 clang 作为编译器，如果有需要请自行修改
$ ./build.sh
```

## 源码结构
```shell
$ tree
.
├── ASTBuilder.cc
├── ASTBuilder.h
├── ASTNode.cc
├── ASTNode.h
├── build.sh
├── CMakeLists.txt
├── conanfile.txt
├── DFA.cc
├── DFA.h
├── LICENSE
├── noncopyable.h
├── README.md
├── Regex.cc
├── Regex.h
└── tests
    ├── CMakeLists.txt
    └── Regex_Unittest.cc

1 directory, 16 files
```

## 使用方法
```C++
Regex r("((ab*|ac*)*|ad*)*");
assert(r.Match("a"));
assert(r.Match(""));
assert(r.Match("abad"));
```
注：具体可参考我测试用例 `Regex_Unittest.cc` 中的方法