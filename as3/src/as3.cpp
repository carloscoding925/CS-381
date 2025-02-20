#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"

int main() {
    raylib::Window window(1000, 700, "CS381 - Assignment 3");
    cs381::SkyBox sky("textures/skybox.png");
    auto camera = raylib::Camera(
        {0, 120, 500},
        {0, 0, 0},
        {0, 1, 0},
        45
    );

    while(!window.ShouldClose()) {
        window.BeginDrawing();
            camera.BeginMode();
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
            camera.EndMode();
        window.EndDrawing();
    }

    return 0;
}