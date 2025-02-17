#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"

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
    raylib::Window window(1000, 700, "CS381 - Assignment 2");
    cs381::SkyBox sky("textures/skybox.png");
    auto camera = raylib::Camera(
        {0, 120, 500},
        {0, 0, 0},
        {0, 1, 0},
        45
    );

    auto rocket = raylib::Model("meshes/rocket.glb");
    rocket.transform = raylib::Matrix::Identity().Scale(30);
    auto truck = raylib::Model("../assets/Kenny Car Kit/truck.glb");
    truck.transform = raylib::Matrix::Identity().Scale(30);

    auto transformerCenter = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({0, 0, 0});
    };
    auto transformerLeft = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({-100, 100, 0}).Scale(1, -1, 1).RotateY(180 * DEG2RAD);
    };
    auto transformerFurtherLeft = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({-200, 0, 0});
    };
    auto transformerFurtherRight = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({200, 0, 0}).RotateY(90 * DEG2RAD);
    };
    auto transformerRight = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({100, 100, 0}).Scale(1, 2, 1).RotateY(270 * DEG2RAD);
    };

    while(!window.ShouldClose()) {
        window.BeginDrawing();
            camera.BeginMode();
                window.ClearBackground(raylib::Color::Black());
                sky.Draw();
                DrawBoundedModel(rocket, transformerCenter);
                DrawBoundedModel(rocket, transformerLeft);
                DrawBoundedModel(truck, transformerFurtherLeft);
                DrawBoundedModel(truck, transformerFurtherRight);
                DrawBoundedModel(truck, transformerRight);
            camera.EndMode();
        window.EndDrawing();
    }

    return 0;
}