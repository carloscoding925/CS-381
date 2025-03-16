#include <raylib-cpp.hpp>
#include "Window.hpp"
#include "../assets/skybox.hpp"
#include "CO.hpp"
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

struct MeshRenderComponent: cs381::Component {
    raylib::Model* model = nullptr;

    MeshRenderComponent(cs381::Entity& e, raylib::Model* model)
        : cs381::Component(e), model(model) {}

    void Tick(float dt) override {
        if (model == nullptr) { return; }

        DrawBoundedModel(*model, [this](raylib::Matrix& matrix){
            auto& transform = Object().Transform();
            return matrix.Translate(transform.position).RotateY(transform.heading);
        });
    }
};

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS 381 - Assignment 6";
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

    std::vector<cs381::Entity> entities;
    entities.emplace_back().AddComponent<MeshRenderComponent>(&rocket);

    while(!window.ShouldClose()) {
        window.BeginDrawing();
            camera.BeginMode();
                window.ClearBackground(raylib::Color::White());
                sky.Draw();

                auto dt = window.GetFrameTime();
                for(auto& e : entities) {
                    e.Tick(dt);
                }
            camera.EndMode();
        window.EndDrawing();
    }

    return 0;
}