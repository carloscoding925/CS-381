#include <raylib-cpp.hpp>
#include "Window.hpp"
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
    model.GetTransformedBoundingBox().Draw(raylib::Color::Black());
    model.transform = backup;
}

void DrawModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    model.transform = backup;
}

struct Entity {
    raylib::Vector3 position;
};

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS 381 - Assignment 5";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 120.0f, -500.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    raylib::Model rocket("../assets/Kenny Space Kit/rocketA.glb");
    rocket.transform = raylib::Matrix::Identity().Scale(30);

    std::vector<Entity> entities;
    entities.emplace_back(raylib::Vector3{0, 0, 0});
    entities.emplace_back(raylib::Vector3{100, 0, 0});
    entities.emplace_back(raylib::Vector3{-100, 0, 0});
    size_t selected_entity = 0;

    while(!window.ShouldClose()) {
        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();

                for(size_t i = 0; i < entities.size(); i++) {
                    auto transformer = [&entities, i](raylib::Transform& t) {
                        return t.Translate(entities[i].position);
                    };
                    if (i == selected_entity) {
                        DrawBoundedModel(rocket, transformer);
                    }
                    else {
                        DrawModel(rocket, transformer);
                    }
                }
            }
            camera.EndMode();
        }
        window.EndDrawing();

        if(raylib::Keyboard::IsKeyPressed(KEY_TAB)) {
            selected_entity = (selected_entity + 1) % entities.size();
        }
    }

    return 0;
}