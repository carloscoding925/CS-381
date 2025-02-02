#include "Color.hpp"
#include <iostream>

#include <raylib-cpp.hpp>

int main() {
    raylib::Window window(800, 400, "CS381 - Assignment 0");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Text text;
    text.SetText("Congratulations! Assignment 0 is complete!");
    text.SetFontSize(30);
    text.SetColor(raylib::Color::Black());

    while(!window.ShouldClose()) {
        int width = window.GetWidth();
        int height = window.GetHeight();
        int textWidth = text.Measure();
        int textHeight = text.GetFontSize();

        float textXPosition = (width - textWidth) / 2;
        float textYPosition = (height - textHeight) / 2;

        window.BeginDrawing();
            window.ClearBackground(raylib::Color::White());
            text.Draw({textXPosition, textYPosition});
        window.EndDrawing();
    }

    return 0;
}