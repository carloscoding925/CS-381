#include <raylib-cpp.hpp>
#include "Window.hpp"
#include "../assets/skybox.hpp"
#include <iostream>
#include "ECS.hpp"

struct Scene {
    struct PhysicsComponent {
        raylib::Vector3 position;
        raylib::Vector3 velocity;
    };

    PhysicsComponent physics[10];
    float heading[10];
    raylib::Model* model[10];
};

void kinematic_physics(Scene& scene, float dt) {
    for (int e = 0; e < 10; ++e) {
        scene.physics[e].position = scene.physics[e].position + scene.physics[e].velocity * dt;
    }
}

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS 381 - Assignment 8";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 120.0f, -500.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    cs381::Scene<cs381::ComponentStorage> scene;
    auto e = scene.CreateEntity();
    scene.AddComponent<raylib::Vector3>(e);

    /*for(cs381::Entity e = 0; e < scene.entityMasks.size(); ++e) {
        if (!scene.HasComponent<Position>(e)) {
            continue;
        }
        if (!scene.HasComponent<Velocity>(e)) {
            continue;
        }

        // do physics on e
    }*/

    while(!window.ShouldClose()) {
        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
            }
            camera.EndMode();
        }
        window.EndDrawing();
    }

    return 0;
}