cmake_minimum_required(VERSION 3.13)
project(T-Reg CXX)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
enable_testing()


file(GLOB CLANG_FORMAT_BIN_NAME /usr/bin/clang-format-[3-9]*)
if(NOT CLANG_FORMAT_BIN_NAME-NOTFOUND)
    message("The clang-format is ${CLANG_FORMAT_BIN_NAME}")
    file(GLOB_RECURSE ALL_SOURCE_FILES *.cc *.h)
    add_custom_target(
        clang-format
        COMMAND ${CLANG_FORMAT_BIN_NAME}
        -style=google
        -i
        ${ALL_SOURCE_FILES}
    )
endif()

set(CMAKE_CXX_STANDARD 11)

add_compile_options(
 -Wall
 -Wno-unused-parameter
 -Woverloaded-virtual
 -Wpointer-arith
 -Wwrite-strings
 -O3
 )

set(REG_SRC 
    ASTNode.cc
    ASTBuilder.cc
    DFA.cc
    Regex.cc)

add_library(Reg STATIC ${REG_SRC})

add_subdirectory(tests)