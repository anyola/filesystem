cmake_minimum_required(VERSION 3.20)
project(filesystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MY_CLANG_TIDY
    clang-tidy;
    -checks=performance-*,bugprone*
)

add_executable(filesystem
    src/filesystem.cpp
    src/file.cpp
    src/directory.cpp
    src/symlink.cpp
)

target_include_directories(filesystem PRIVATE include)

set_target_properties(filesystem PROPERTIES
    CXX_CLANG_TIDY "${MY_CLANG_TIDY}"
)

target_compile_options(filesystem PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(filesystem PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-omit-frame-pointer
    )

    target_link_options(filesystem PRIVATE
        -fsanitize=address
        -fsanitize=undefined
    )
endif()


# GoogleTest
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


# Tests
enable_testing()

add_executable(filesystem_tests
    src/filesystem.cpp
    src/file.cpp
    src/directory.cpp
    src/symlink.cpp
    tests/filesystem_tests.cpp
)

target_include_directories(filesystem_tests PRIVATE include)

set_target_properties(filesystem_tests PROPERTIES
    CXX_CLANG_TIDY "${MY_CLANG_TIDY}"
)

target_link_libraries(filesystem_tests
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(filesystem_tests PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

include(GoogleTest)

gtest_discover_tests(filesystem_tests)