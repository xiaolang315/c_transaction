file(GLOB_RECURSE all_files
        *.cpp
        *.c
)

# CppUTest
CPMAddPackage(
    CppUTest
    GITHUB_REPOSITORY cpputest/cpputest
    GIT_TAG        latest-passing-build # or use release tag, eg. v3.8
    OPTIONS " TESTS OFF "
)
include(Dart)

add_executable(c_trans_test ${all_files})
target_link_libraries(c_trans_test PRIVATE CppUTest CppUTestExt lib)

add_test(NAME test COMMAND c_trans_test)