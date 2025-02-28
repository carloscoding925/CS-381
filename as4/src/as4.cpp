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

    raylib::Model toilet("../assets/Kenny-Furniture-Kit/toilet.glb");
    toilet.transform = raylib::Matrix::Identity().Scale(5);
    raylib::Model hotdog("../assets/Kenny-Food-Kit/hot-dog-raw.glb");
    hotdog.transform = raylib::Matrix::Identity().Scale(5);

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.png");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 10.0f, 30.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    while(!window.ShouldClose()) {

        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::White());
            camera.BeginMode();
            {
                sky.Draw();
                grass.Draw({});
                DrawBoundedModel(toilet, [](raylib::Transform t) {
                    return t.Translate(raylib::Vector3{0.0f, 0.0f, 0.0f});
                });
                DrawBoundedModel(hotdog, [](raylib::Transform t) {
                    return t.Translate(raylib::Vector3{0.0f, 0.0f, 0.0f});
                });
            }
            camera.EndMode();
        }
        window.EndDrawing();

    }

    return 0;
}