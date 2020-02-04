cmake_minimum_required(VERSION 3.1)

project(c_transaction)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 14)

include_directories(
    "/Users/zhangchao/code/googletest/googletest/include/"
        "/usr/local/include"
    "src/"
)

file(GLOB_RECURSE all_files
src/*.cpp
src/*.c
test/*.cpp
)

link_directories(
    "/usr/local/lib"
    "/Users/zhangchao/code/googletest/googletest/build/lib/"
)

add_executable(c_transaction  ${all_files})
target_link_libraries(c_transaction CppUTest CppUTestExt)
