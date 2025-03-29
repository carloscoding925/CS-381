#include <raylib-cpp.hpp>
#include <BufferedRaylib.hpp>
#include "Window.hpp"
#include "../assets/skybox.hpp"
#include "CO.hpp"
#include <iostream>

template <typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    model.GetTransformedBoundingBox().Draw(raylib::Color::White());
    model.transform = backup;
}

void DrawModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    model.transform = backup;
}

int main() {

    return 0;
}