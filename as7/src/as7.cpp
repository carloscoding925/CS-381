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

struct MeshRenderComponent: cs381::Component {
    raylib::Model* model = nullptr;

    MeshRenderComponent(cs381::Entity& e, raylib::Model* model)
        : cs381::Component(e), model(model) {}

    void Tick(float dt) {
        if (model == nullptr) { return; }

        DrawModel(*model, [this](raylib::Matrix& matrix){
            auto& transform = Object().Transform();
            return matrix.Translate(transform.position).RotateY(transform.heading);
        });
    }
};

struct GravityComponent: cs381::Component {
    raylib::Vector3 gravity = {0.0f, 0.0f, 0.0f};

    GravityComponent(cs381::Entity& e)
        : cs381::Component(e) {}

    void Tick(float dt) override {
        auto& transform = Object().Transform();
        transform.position = transform.position + gravity * (10 * dt);
    }

    void Jump() {
        auto& transform = Object().Transform();
        transform.position.y = transform.position.y + 40;
    }

    void Start() {
        gravity = raylib::Vector3{0.0f, -9.81f, 0.0f};
    }

    void Stop() {
        gravity = raylib::Vector3{0.0f, 0.0f, 0.0f};
    }
};

struct MovementComponent: cs381::Component {
    raylib::Vector3 velocity = {0.0f, 0.0f, 0.0f};
    float radians = DEG2RAD * raylib::Degree(0);
    float speed = 0.0f;
    int heightChange = 0;
    bool addHeight = false;

    MovementComponent(cs381::Entity& e)
        : cs381::Component(e) {}

    void Tick(float dt) override {
        velocity = raylib::Vector3{ cos(radians) * speed, 0, -sin(radians) * speed };
        auto& transform = Object().Transform();
        transform.position = transform.position + velocity * dt;

        if (transform.position.x > 250) {
            Reset();
            RandomizeHeight();
        }
    }
    
    void Start() {
        speed = 100.0f;
    }

    void Stop() {
        speed = 0.0f;
    }

    void Reset() {
        auto& transform = Object().Transform();
        transform.position.x = -250;
    }
    
    void RandomizeHeight() {
        auto& transform = Object().Transform();
        float initialY = transform.position.y;
    
        if (addHeight) {
            transform.position.y = transform.position.y + heightChange;
        } else {
            transform.position.y = transform.position.y - heightChange;
        }
    
        if (transform.position.y > initialY + 100) {
            transform.position.y = initialY + 100;
        } else if (transform.position.y < initialY - 100) {
            transform.position.y = initialY - 100;
        }
    
        addHeight = !addHeight;
    }
};

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS 381 - Assignment 7";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 120.0f, -500.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    raylib::Model fireTruck("../assets/Kenny Car Kit/firetruck.glb");
    raylib::Model police("../assets/Kenny Car Kit/police.glb");
    fireTruck.transform = raylib::Matrix::Identity().Scale(20).RotateY(raylib::Degree(270));
    police.transform = raylib::Matrix::Identity().Scale(20).RotateY(raylib::Degree(90));

    std::vector<cs381::Entity> entities;

    cs381::Entity& fireTruckEntity = entities.emplace_back();
    fireTruckEntity.AddComponent<MeshRenderComponent>(&fireTruck);
    fireTruckEntity.AddComponent<GravityComponent>();
    fireTruckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{0, 0, 0};

    cs381::Entity& topCar = entities.emplace_back();
    topCar.AddComponent<MeshRenderComponent>(&police);
    topCar.AddComponent<MovementComponent>();
    topCar.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-250, 80, 0};

    cs381::Entity& bottomCar = entities.emplace_back();
    bottomCar.AddComponent<MeshRenderComponent>(&police);
    bottomCar.AddComponent<MovementComponent>();
    bottomCar.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-250, -80, 0};

    bool spacePressed = false;
    bool started = false;

    while(!window.ShouldClose()) {
        if (!started && raylib::Keyboard::IsKeyDown(KEY_SPACE)) {
            entities[0].GetComponent<GravityComponent>()->get().Start();
            entities[1].GetComponent<MovementComponent>()->get().Start();
            entities[2].GetComponent<MovementComponent>()->get().Start();
            started = true;
        }
        if (raylib::Keyboard::IsKeyDown(KEY_SPACE) && !spacePressed) {
            entities[0].GetComponent<GravityComponent>()->get().Jump();
        }
        spacePressed = raylib::Keyboard::IsKeyDown(KEY_SPACE);

        int heightChange = rand() % 100;
        entities[1].GetComponent<MovementComponent>()->get().heightChange = heightChange;
        entities[2].GetComponent<MovementComponent>()->get().heightChange = heightChange;

        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();

                auto dt = window.GetFrameTime();
                for(auto& e : entities) {
                    e.Tick(dt);
                }
            }
            camera.EndMode();
        }
        window.EndDrawing();
    }

    return 0;
}