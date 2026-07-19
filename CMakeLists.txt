cmake_minimum_required(VERSION 3.30)
project(cpp-playground LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(CppPlayground STATIC src/cpp_playground/cpp_playground.cpp)
target_include_directories(CppPlayground PUBLIC include)

find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)
enable_testing()
add_subdirectory(tests)
add_subdirectory(src)

option(CPP_PLAYGROUND_BUILD_DOCS "Build documentation" OFF)
if(CPP_PLAYGROUND_BUILD_DOCS)
    add_subdirectory(doc)
endif()
