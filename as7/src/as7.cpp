#include <raylib-cpp.hpp>
#include <BufferedRaylib.hpp>
#include "Window.hpp"
#include "../assets/skybox.hpp"
#include "CO.hpp"
#include <iostream>
#include "FlappyGui.h"

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
    bool addScore = false;
    FlappyCounterState* flappyCounterState;
    Sound flappySound;
    bool playSound = false;

    MovementComponent(cs381::Entity& e, FlappyCounterState* state, Sound sound)
        : cs381::Component(e), flappyCounterState(state), flappySound(sound) {}

    void Tick(float dt) override {
        velocity = raylib::Vector3{ cos(radians) * speed, 0, -sin(radians) * speed };
        auto& transform = Object().Transform();
        transform.position = transform.position + velocity * dt;

        if (transform.position.x > 250) {
            Reset();
            RandomizeHeight();
        }

        if (transform.position.x > 0 && !addScore) {
            flappyCounterState->score = flappyCounterState->score + 0.5f;
            addScore = true;
            if (playSound) {
                PlaySound(flappySound);
            }
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
        addScore = false;
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

struct CollisionComponent: cs381::Component {
    cs381::Entity* topCar;
    cs381::Entity* bottomCar;
    bool collisionDetected = false;

    CollisionComponent(cs381::Entity& e, cs381::Entity* topCar, cs381::Entity* bottomCar)
        : cs381::Component(e), topCar(topCar), bottomCar(bottomCar) {}

    void Tick(float dt) override {
        auto& position = Object().Transform().position;
        auto& topCarPosition = topCar->Transform().position;
        auto& bottomCarPosition = bottomCar->Transform().position;

        if (std::fabs(position.x - topCarPosition.x) < 20.0f && position.y > topCarPosition.y) {
            collisionDetected = true;
        }
        else if (std::fabs(position.x - bottomCarPosition.x) < 20.0f && position.y < bottomCarPosition.y) {
            collisionDetected = true;
        }
        else if (position.y < -300.0f) {
            collisionDetected = true;
        }
    }
};

struct GameStateComponent: cs381::Component {
    GameStateComponent(cs381::Entity& e)
        : cs381::Component(e) {}

    void Tick(float dt) override {

    }

    void StopTruck() {
        Object().GetComponent<GravityComponent>()->get().Stop();
    }

    void StopCars() {
        Object().GetComponent<MovementComponent>()->get().Stop();
    }

    void ResetTruck() {
        auto& transform = Object().Transform();
        transform.position.x = 0;
        transform.position.y = 0;
    }
    
    void ResetTopCar() {
        auto& transform = Object().Transform();
        transform.position.x = -250;
        transform.position.y = 80;
    }

    void ResetBottomCar() {
        auto& transform = Object().Transform();
        transform.position.x = -250;
        transform.position.y = -80;
    }
};

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "Flappy Fire Truck";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 120.0f, -500.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    FlappyCounterState flappyCounterState = InitFlappyCounter();

    raylib::AudioDevice audioDevice;
    Sound flappySound = LoadSound("../assets/audio/flappy-sound.mp3");

    raylib::Model fireTruck("../assets/Kenny Car Kit/firetruck.glb");
    raylib::Model police("../assets/Kenny Car Kit/police.glb");
    fireTruck.transform = raylib::Matrix::Identity().Scale(20).RotateY(raylib::Degree(270));
    police.transform = raylib::Matrix::Identity().Scale(20).RotateY(raylib::Degree(90));

    std::vector<cs381::Entity> entities;

    cs381::Entity& topCar = entities.emplace_back();
    topCar.AddComponent<MeshRenderComponent>(&police);
    topCar.AddComponent<MovementComponent>(&flappyCounterState, flappySound);
    topCar.AddComponent<GameStateComponent>();
    topCar.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-250, 80, 0};

    cs381::Entity& bottomCar = entities.emplace_back();
    bottomCar.AddComponent<MeshRenderComponent>(&police);
    bottomCar.AddComponent<MovementComponent>(&flappyCounterState, flappySound);
    bottomCar.AddComponent<GameStateComponent>();
    bottomCar.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-250, -80, 0};
    bottomCar.GetComponent<MovementComponent>()->get().playSound = true;

    cs381::Entity& fireTruckEntity = entities.emplace_back();
    fireTruckEntity.AddComponent<MeshRenderComponent>(&fireTruck);
    fireTruckEntity.AddComponent<GravityComponent>();
    fireTruckEntity.AddComponent<CollisionComponent>(&entities[0], &entities[1]);
    fireTruckEntity.AddComponent<GameStateComponent>();
    fireTruckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{0, 0, 0};

    bool spacePressed = false;
    bool started = false;
    flappyCounterState.showStartScreen = true;

    while(!window.ShouldClose()) {
        SetSoundVolume(flappySound, 0.5f);

        if (!started && raylib::Keyboard::IsKeyDown(KEY_SPACE)) {
            entities[0].GetComponent<MovementComponent>()->get().Start();
            entities[1].GetComponent<MovementComponent>()->get().Start();
            entities[2].GetComponent<GravityComponent>()->get().Start();
            started = true;
            flappyCounterState.showStartScreen = false;
            flappyCounterState.score = 0.0f;
        }
        if (raylib::Keyboard::IsKeyDown(KEY_SPACE) && !spacePressed && !entities[2].GetComponent<CollisionComponent>()->get().collisionDetected) {
            entities[2].GetComponent<GravityComponent>()->get().Jump();
        }
        spacePressed = raylib::Keyboard::IsKeyDown(KEY_SPACE);

        int heightChange = rand() % 100;
        entities[0].GetComponent<MovementComponent>()->get().heightChange = heightChange;
        entities[1].GetComponent<MovementComponent>()->get().heightChange = heightChange;

        if (entities[2].GetComponent<CollisionComponent>()->get().collisionDetected) {
            entities[0].GetComponent<GameStateComponent>()->get().StopCars();
            entities[1].GetComponent<GameStateComponent>()->get().StopCars();
            entities[2].GetComponent<GameStateComponent>()->get().StopTruck();
            flappyCounterState.showGameOverScreen = true;
        }

        if (entities[2].GetComponent<CollisionComponent>()->get().collisionDetected && raylib::Keyboard::IsKeyDown(KEY_R)) {
            entities[0].GetComponent<GameStateComponent>()->get().ResetTopCar();
            entities[1].GetComponent<GameStateComponent>()->get().ResetBottomCar();
            entities[2].GetComponent<GameStateComponent>()->get().ResetTruck();

            entities[2].GetComponent<CollisionComponent>()->get().collisionDetected = false;
            started = false;
            flappyCounterState.showGameOverScreen = false;
        }

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
            GuiFlappyCounter(&flappyCounterState);
        }
        window.EndDrawing();
    }

    UnloadSound(flappySound);
    audioDevice.Close();

    return 0;
}