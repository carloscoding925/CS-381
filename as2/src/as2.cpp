#include <raylib-cpp.hpp>
#include "skybox.hpp"

template<typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    model.transform = backup;
}

int main() {
    raylib::Window window(800, 600, "CS381 - Assignment 2");

    raylib::Model cube = raylib::Mesh::Cube(30, 30, 30).LoadModelFrom();
    auto camera = raylib::Camera(
        {0, 120, -500},
        {0, 0, 0},
        {0, 1, 0},
        45
    );

    auto rocket = raylib::Model("meshes/rocket.glb");
    rocket.transform = raylib::Matrix::Identity().Scale(30);

    cs381::SkyBox sky("textures/skybox.png");

    while(!window.ShouldClose()) {
        window.BeginDrawing();
            camera.BeginMode();
                window.ClearBackground(raylib::Color::Black());
                sky.Draw();
                //cube.Draw({0, 0, 0});
                auto transformer = [](raylib::Matrix& transform) -> raylib::Matrix{
                    return transform.Translate({100, 0, 0});
                };
                DrawBoundedModel(rocket, transformer);
                //rocket.Draw({});
            camera.EndMode();
        window.EndDrawing();
    }

    return 0;
}