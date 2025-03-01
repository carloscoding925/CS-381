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

bool checkCollision(raylib::Vector3& toilet, raylib::Vector3& hotdog) {
    float radius = 5.0f;
    if (std::fabs(toilet.x - hotdog.x) < radius && std::fabs(toilet.z - hotdog.z) < radius) {
        return true;
    }

    return false;
}

raylib::Vector3 generateNewLocation() {
    float x = std::rand() % 200 - 100;
    float z = std::rand() % 200 - 100;
    return raylib::Vector3{x, 0.0f, z};
}

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "The Rizzler & Big AJ Present: Skibidi Toilet Glizzy Gobbler";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model toilet("../assets/Kenny-Furniture-Kit/toilet.glb");
    toilet.transform = raylib::Matrix::Identity().Scale(5);
    raylib::Model hotdog("../assets/Kenny-Food-Kit/meat-sausage.glb");
    hotdog.transform = raylib::Matrix::Identity().Scale(15);

    raylib::Model floor = raylib::Mesh::Plane(200, 200, 1, 1).LoadModelFrom();
    raylib::Texture floorTexture = raylib::Texture("../assets/textures/floor.png");
    floor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTexture;

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 20.0f, 40.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    raylib::Vector3 toiletPosition = { 0.0f, 0.0f, 0.0f };
    raylib::Vector3 hotdogPosition = { 0.0f, 0.0f, -50.0f };

    float targetSpeed = 0;
    float toiletSpeed = 0;
    float toiletHeading = 0;
    float targetHeading = 0;
    float toiletYTarget = 0;

    int hotdogCounter = 0;

    while(!window.ShouldClose()) {
        if (raylib::Keyboard::IsKeyDown(KEY_W)) {
            targetSpeed = 20.0f;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_S)) {
            targetSpeed = -20.0f;
        } 
        else {
            targetSpeed = 0.0f;
        }

        if (raylib::Keyboard::IsKeyDown(KEY_A)) {
            targetHeading = targetHeading + 0.04f;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_D)) {
            targetHeading = targetHeading - 0.04;
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_SPACE)) {
            toiletYTarget = 4.0f;
        }
        else if (toiletPosition.y > 3.5f){
            toiletYTarget = 0;
        }

        float radians = DEG2RAD * toiletHeading;
        toiletSpeed = std::lerp(toiletSpeed, targetSpeed, window.GetFrameTime());
        toiletHeading = std::lerp(toiletHeading, targetHeading, window.GetFrameTime());
        toiletPosition.y = std::lerp(toiletPosition.y, toiletYTarget, window.GetFrameTime() * 2);
        raylib::Vector3 velocity = { cos(radians) * toiletSpeed, 0, -sin(radians) * toiletSpeed };
        toiletPosition = toiletPosition + velocity * window.GetFrameTime();

        camera.position = toiletPosition + raylib::Vector3{0.0f, 20.0f, 40.0f};
        camera.target = toiletPosition;

        if (checkCollision(toiletPosition, hotdogPosition)) {
            hotdogPosition = generateNewLocation();
            hotdogCounter++;
        }

        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::White());
            camera.BeginMode();
            {
                sky.Draw();
                floor.Draw({});
                DrawBoundedModel(toilet, [&toiletPosition, &toiletHeading](raylib::Transform t) {
                    return t.Translate(toiletPosition).RotateY(raylib::Degree(toiletHeading));
                });
                DrawBoundedModel(hotdog, [&hotdogPosition](raylib::Transform t) {
                    return t.Translate(hotdogPosition);
                });
            }
            camera.EndMode();
        }
        window.EndDrawing();

    }

    return 0;
}