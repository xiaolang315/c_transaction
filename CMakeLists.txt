cmake_minimum_required(VERSION 3.16)

project(c_transaction)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME mcl
        GITHUB_REPOSITORY ccup/mcl
        GIT_TAG master
)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE all_files
        src/*.cpp
        src/*.c
        test/*.cpp
        log/src/*.c
)

add_executable(c_transaction  ${all_files})

target_link_directories(c_transaction PRIVATE "/usr/local/lib")
target_include_directories(c_transaction PRIVATE
        "/usr/local/include"
        "src/"
        "log/src"
        )
target_link_libraries(c_transaction CppUTest CppUTestExt Cello mcl)
