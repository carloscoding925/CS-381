cmake_minimum_required(VERSION 3.18)
project(as9 C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib-cpp)
include(includeable.cmake)

add_executable(as9 src/as9.cpp src/implementations.cpp assets/skybox.cpp)
target_link_libraries(as9 PUBLIC raylib raylib_cpp raygui raylib::buffered)

make_includeable(assets/shaders/cubemap.fs generated/cubemap.fs)
make_includeable(assets/shaders/cubemap.vs generated/cubemap.vs)
make_includeable(assets/shaders/skybox.fs generated/skybox.fs)
make_includeable(assets/shaders/skybox.vs generated/skybox.vs)

configure_file(assets/textures/skybox.png textures/skybox.png COPYONLY)
configure_file("assets/Kenny-Furniture-Kit/plantSmall1.glb" meshes/plantSmall1.glb COPYONLY)
