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
    model.GetTransformedBoundingBox().Draw(raylib::Color::White());
    model.transform = backup;
}

void DrawModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    model.transform = backup;
}

struct Entity {
    raylib::Model* model;

    float modelSpeed;
    float targetSpeed;
    float targetHeading;
    float modelHeading;
    raylib::Vector3 position;

    Entity(const raylib::Vector3& position, raylib::Model* model, float modelSpeed, float targetSpeed, float targetHeading, float modelHeading)
    : 
    position(position), model(model), modelSpeed(modelSpeed), targetSpeed(targetSpeed), targetHeading(targetHeading), modelHeading(modelHeading) {}
};

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS 381 - Assignment 5";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 120.0f, -500.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    raylib::Model grass = raylib::Mesh::Plane(1000, 1000, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.png");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    raylib::Model rocket("../assets/Kenny Space Kit/rocketA.glb");
    raylib::Model truck("../assets/Kenny Car Kit/truck.glb");
    rocket.transform = raylib::Matrix::Identity().Scale(30);
    truck.transform = raylib::Matrix::Identity().Scale(30).RotateY(raylib::Degree(90));

    std::vector<Entity> entities;
    entities.emplace_back(raylib::Vector3{0, 0, 0}, &rocket, 0.0f, 0.0f, 0.0f, 0.0f);
    entities.emplace_back(raylib::Vector3{100, 0, 0}, &truck, 0.0f, 0.0f, 0.0f, 0.0f);
    entities.emplace_back(raylib::Vector3{-100, 0, 0}, &truck, 0.0f, 0.0f, 0.0f, 0.0f);
    size_t selected_entity = 0;

    float timer = 0.0f;
    bool isLeftPressed = false;
    bool isRightPressed = false;

    while(!window.ShouldClose()) {
        if (raylib::Keyboard::IsKeyDown(KEY_W) && timer <= 0) {
            timer = 1.0;
            entities[selected_entity].targetSpeed = 10.0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_S) && timer <= 0) {
            timer = 1.0;
            entities[selected_entity].targetSpeed = 0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_SPACE) && timer <= 0) {
            timer = 1.0;
            entities[selected_entity].targetSpeed = 0;
            entities[selected_entity].modelSpeed = 0;
        }

        timer = timer - window.GetFrameTime();

        if (raylib::Keyboard::IsKeyDown(KEY_A) && !isLeftPressed) {
            entities[selected_entity].targetHeading = entities[selected_entity].targetHeading + 30;
        }
        isLeftPressed = raylib::Keyboard::IsKeyDown(KEY_A);
        if (raylib::Keyboard::IsKeyDown(KEY_D) && !isRightPressed) {
            entities[selected_entity].targetHeading = entities[selected_entity].targetHeading - 30;
        }
        isRightPressed = raylib::Keyboard::IsKeyDown(KEY_D);

        for (auto& entity : entities) {
            float radians = DEG2RAD * entity.modelHeading;
            entity.modelSpeed = std::lerp(entity.modelSpeed, entity.targetSpeed, window.GetFrameTime());
            entity.modelHeading = std::lerp(entity.modelHeading, entity.targetHeading, window.GetFrameTime());
            raylib::Vector3 velocity = { cos(radians) * entity.modelSpeed, 0, -sin(radians) * entity.modelSpeed };
            entity.position = entity.position + velocity * window.GetFrameTime();
        }

        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
                grass.Draw({});

                for(size_t i = 0; i < entities.size(); i++) {
                    if (i == selected_entity) {
                        DrawBoundedModel(*entities[i].model, [&entities, i](raylib::Transform& t) {
                            return t.Translate(entities[i].position).RotateY(raylib::Degree(entities[i].modelHeading));
                        });
                    }
                    else {
                        DrawModel(*entities[i].model, [&entities, i](raylib::Transform& t) {
                            return t.Translate(entities[i].position).RotateY(raylib::Degree(entities[i].modelHeading));
                        });
                    }
                }
            }
            camera.EndMode();
        }
        window.EndDrawing();

        if (raylib::Keyboard::IsKeyPressed(KEY_TAB)) {
            selected_entity = (selected_entity + 1) % entities.size();
        }
    }

    return 0;
}