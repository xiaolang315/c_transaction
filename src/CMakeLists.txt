file(GLOB_RECURSE all_files
        *.cpp
        *.c
)

CPMAddPackage(
        NAME mcl
        GITHUB_REPOSITORY ccup/mcl
        GIT_TAG master
        OPTIONS "ENABEL_TEST OFF"
)

add_library(lib ${all_files})
target_link_libraries(lib PUBLIC mcl)
target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})