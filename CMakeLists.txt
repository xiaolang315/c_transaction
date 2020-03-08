cmake_minimum_required(VERSION 3.1)

project(c_transaction)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -fmodules-ts)
include_directories(
        "/usr/local/include"
    "src/"
        "log/src"
)

file(GLOB_RECURSE all_files
src/*.cpp
src/*.c
test/*.cpp
        log/src/*.c
)

link_directories(
    "/usr/local/lib"
)

add_executable(c_transaction  ${all_files})
target_link_libraries(c_transaction CppUTest CppUTestExt Cello)
