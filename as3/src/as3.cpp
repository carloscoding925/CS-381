#include "Color.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"
#include <iostream>

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
    const int screenWidth = 1000;
    const int screenHeight = 700;
    const std::string title = "CS381 - Assignment 3";

    raylib::Window window(screenWidth, screenHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model model("../assets/Kenny Car Kit/cone.glb");
    model.transform = raylib::Matrix::Identity().Scale(5);

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    float targetSpeed = 5;
    float coneHeading = 45;
    float coneSpeed = 0;
    raylib::Vector3 conePosition = { 0.0f, 0.0f, 0.0f };

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 10.0f, 30.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    float timer = 0;

    while(!window.ShouldClose()) {

        coneSpeed = std::lerp(coneSpeed, targetSpeed, window.GetFrameTime());
        raylib::Vector3 velocity = { cos(raylib::Degree(coneHeading)) * coneSpeed, 0, -sin(raylib::Degree(coneHeading)) * coneSpeed };
        conePosition = conePosition + velocity * window.GetFrameTime();

        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::White());
            camera.BeginMode();
            {
                sky.Draw();
                grass.Draw({});
                DrawBoundedModel(model, [&conePosition, &coneHeading](raylib::Transform t) {
                    return t.Translate(conePosition).RotateY(raylib::Degree(coneHeading));
                });
            }
            camera.EndMode();

        }
        window.EndDrawing();

    }

    return 0;
}