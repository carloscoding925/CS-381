#include "Color.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"
#include <iostream>
#include <raymath.h>

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS381 - Assignment 4";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    while(!window.ShouldClose()) {

        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::White());
        }
        window.EndDrawing();

    }

    return 0;
}