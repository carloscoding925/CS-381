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
    const int screenWidth = 1000;
    const int screenHeight = 700;
    const std::string title = "CS381 - Assignment 3";

    raylib::Window window(screenWidth, screenHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model truck("../assets/Kenny Car Kit/truck.glb");
    truck.transform = raylib::Matrix::Identity().Scale(5);

    raylib::Model wheels("../assets/Kenny Car Kit/wheel-default.glb");
    wheels.transform = raylib::Matrix::Identity().Scale(5);

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    float targetSpeed = 0;
    float truckHeading = 0;
    float truckSpeed = 0;
    float truckYTarget = 0;
    raylib::Vector3 truckPosition = { 0.0f, 0.0f, 0.0f };

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 10.0f, 30.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    float timer = 0;
    bool isLeftPressed = false;
    bool isRightPressed = false;

    while(!window.ShouldClose()) {

        if (raylib::Keyboard::IsKeyDown(KEY_UP) && timer <= 0) {
            timer = 1.0;
            targetSpeed = 2.5;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_DOWN) && timer <= 0) {
            timer = 1.0;
            targetSpeed = 0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_SPACE) && timer <= 0) {
            timer = 1.0;
            targetSpeed = 0;
            truckSpeed = 0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_W) && timer <= 0) {
            timer = 1.0;
            truckYTarget = 3.0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_S) && timer <= 0) {
            timer = 1.0;
            truckYTarget = 0.0;
        }
        timer -= window.GetFrameTime();
        
        if (raylib::Keyboard::IsKeyDown(KEY_LEFT) && !isLeftPressed) {
            truckHeading = truckHeading + 10;
        }
        isLeftPressed = raylib::Keyboard::IsKeyDown(KEY_LEFT);
        if (raylib::Keyboard::IsKeyDown(KEY_RIGHT) && !isRightPressed) {
            truckHeading = truckHeading - 10;
        }
        isRightPressed = raylib::Keyboard::IsKeyDown(KEY_RIGHT);

        float radians = DEG2RAD * truckHeading;
        truckSpeed = std::lerp(truckSpeed, targetSpeed, window.GetFrameTime());
        raylib::Vector3 velocity = { cos(radians) * truckSpeed, 0, -sin(radians) * truckSpeed };
        truckPosition = truckPosition + velocity * window.GetFrameTime();

        truckPosition.y = std::lerp(truckPosition.y, truckYTarget, window.GetFrameTime());

        raylib::Vector3 frontRightWheelPos = truckPosition + raylib::Vector3{4.6f * cos(radians) + 2.5f * sin(radians), 0, 4.6f * -sin(radians) - 2.5f * -cos(radians)};
        raylib::Vector3 backRightWheelPos = truckPosition + raylib::Vector3{-3.8f * cos(radians) + 2.5f * sin(radians), 0, -3.8f * -sin(radians) - 2.5f * -cos(radians)};
        raylib::Vector3 frontLeftWheelPos = truckPosition + raylib::Vector3{4.6f * cos(radians) - 2.5f * sin(radians), 0, 4.6f * -sin(radians) + 2.5f * -cos(radians)};
        raylib::Vector3 backLeftWheelPos = truckPosition + raylib::Vector3{-3.8f * cos(radians) - 2.5f * sin(radians), 0, -3.8f * -sin(radians) + 2.5f * -cos(radians)};

        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::White());
            camera.BeginMode();
            {
                sky.Draw();
                grass.Draw({});
                // Truck
                DrawBoundedModel(truck, [&truckPosition, &truckHeading](raylib::Transform t) {
                    return t.Translate(truckPosition).RotateY(raylib::Degree(truckHeading + 90));
                });
                // Front Right Wheel
                DrawBoundedModel(wheels, [&truckHeading, &frontRightWheelPos](raylib::Transform t) {
                    return t.Translate(frontRightWheelPos).RotateY(raylib::Degree(truckHeading + 270));
                });
                // Back Right Wheel
                DrawBoundedModel(wheels, [&truckHeading, &backRightWheelPos](raylib::Transform t) {
                    return t.Translate(backRightWheelPos).RotateY(raylib::Degree(truckHeading + 270));
                });
                // Front Left Wheel
                DrawBoundedModel(wheels, [&truckHeading, &frontLeftWheelPos](raylib::Transform t) {
                    return t.Translate(frontLeftWheelPos).RotateY(raylib::Degree(truckHeading + 90));
                });
                // Back Left Wheel
                DrawBoundedModel(wheels, [&truckHeading, &backLeftWheelPos](raylib::Transform t) {
                    return t.Translate(backLeftWheelPos).RotateY(raylib::Degree(truckHeading + 90));
                });
            }
            camera.EndMode();

        }
        window.EndDrawing();

    }

    return 0;
}