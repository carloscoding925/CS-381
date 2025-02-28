#include "Color.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"
#include <iostream>
#include <raymath.h>

template<typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    auto box = model.GetTransformedBoundingBox();
    DrawBoundingBox(box, raylib::Color::Black());
    model.transform = backup;
}

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "Skibidi Toilet Glizzy Gobler";
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