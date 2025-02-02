#include "Color.hpp"
#include <iostream>

#include <raylib-cpp.hpp>

int main() {
    raylib::Window window(800, 400, "Hello World");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Text text;
    text.SetText("Hello World!");
    text.SetFontSize(30);

    while(!window.ShouldClose()) {
        window.BeginDrawing();
            window.ClearBackground(raylib::Color::Blue());
            text.Draw({100, 0});
        window.EndDrawing();
    }

    return 0;
}