#include <raylib-cpp.hpp>
#include "Window.hpp"
#include "../assets/skybox.hpp"
#include <iostream>

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS 381 - Assignment 8";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 120.0f, -500.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    while(!window.ShouldClose()) {
        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
            }
            camera.EndMode();
        }
        window.EndDrawing();
    }

    return 0;
}