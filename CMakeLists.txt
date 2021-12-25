cmake_minimum_required(VERSION 3.16)

project(c_transaction)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

option(C_TRANS_ENABLE_TEST "Build tests" ON)

if(C_TRANS_ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
