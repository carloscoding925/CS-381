cmake_minimum_required(VERSION 3.18)
project(as2 C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib-cpp)
include(includeable.cmake)

add_executable(as2 src/as2.cpp assets/skybox.cpp)
target_link_libraries(as2 PUBLIC raylib raylib_cpp)

make_includeable(assets/shaders/cubemap.fs generated/cubemap.fs)
make_includeable(assets/shaders/cubemap.vs generated/cubemap.vs)
make_includeable(assets/shaders/skybox.fs generated/skybox.fs)
make_includeable(assets/shaders/skybox.vs generated/skybox.vs)

configure_file(assets/textures/skybox.png textures/skybox.png COPYONLY)
configure_file("assets/Kenny Space Kit/rocketA.glb" meshes/rocket.glb COPYONLY)
configure_file("assets/Kenny Car Kit/truck.glb" meshes/truck.glb COPYONLY)