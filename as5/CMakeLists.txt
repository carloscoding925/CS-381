cmake_minimum_required(VERSION 3.18)
project(as5 C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib-cpp)
include(includeable.cmake)

add_executable(as5 src/as5.cpp assets/skybox.cpp)
target_link_libraries(as5 PUBLIC raylib raylib_cpp)

make_includeable(assets/shaders/cubemap.fs generated/cubemap.fs)
make_includeable(assets/shaders/cubemap.vs generated/cubemap.vs)
make_includeable(assets/shaders/skybox.fs generated/skybox.fs)
make_includeable(assets/shaders/skybox.vs generated/skybox.vs)

configure_file(assets/textures/skybox.png textures/skybox.png COPYONLY)
configure_file("assets/Kenny Car Kit/truck.glb" meshes/truck.glb COPYONLY)
configure_file("assets/Kenny Space Kit/rocketA.glb" meshes/rocketA.glb COPYONLY)