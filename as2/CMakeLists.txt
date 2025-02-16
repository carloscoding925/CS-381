cmake_minimum_required(VERSION 3.18)
project(as2 C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib-cpp)

add_executable(as2 src/as2.cpp)
target_link_libraries(as2 PUBLIC raylib raylib_cpp)