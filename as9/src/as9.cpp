#include <raylib-cpp.hpp>
#include <BufferedRaylib.hpp>
#include "Window.hpp"
#include "../assets/skybox.hpp"
#include <iostream>
#include "ECS.hpp"
#include "BroccoliCounter.h"

size_t globalComponentCounter = 0;

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

struct RenderComponent {
    raylib::Model* model = nullptr;
};

struct TransformComponent {
    raylib::Vector3 position = {0.0f, 0.0f, 0.0f};
    raylib::Degree heading = 0;
    int scale = 0;
};

struct InputStateComponent {
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
};

struct PhysicsComponent {
    raylib::Vector3 velocity = {0.0f, 0.0f, 0.0f};
    float speed = 0.0f;
    float targetSpeed = 0.0f;
    raylib::Degree targetHeading = 0;
    float rotationSpeed = 0.0f;
};

void RenderSystem(cs381::Scene<cs381::ComponentStorage>& scene) {
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<RenderComponent>(e)) {
            continue;
        }
        if (!scene.HasComponent<TransformComponent>(e)) {
            continue;
        }

        auto& renderComponent = scene.GetComponent<RenderComponent>(e);
        auto& transformComponent = scene.GetComponent<TransformComponent>(e);

        DrawModel(*renderComponent.model, [&transformComponent](const raylib::Transform& m) -> raylib::Transform {
            return m.Translate(transformComponent.position).Scale(transformComponent.scale).RotateY(transformComponent.heading);
        });
    }
}

void InputSystem(cs381::Scene<cs381::ComponentStorage>& scene, raylib::BufferedInput& bufferedInput) {
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<InputStateComponent>(e)) {
            return;
        }
        if (!scene.HasComponent<PhysicsComponent>(e)) {
            return;
        }
    
        auto& physicsComponent = scene.GetComponent<PhysicsComponent>(e);
        auto& inputStateComponent = scene.GetComponent<InputStateComponent>(e);
        auto& transformComponent = scene.GetComponent<TransformComponent>(e);

        int playerEntity = 0;

        bufferedInput["Forward"].AddPressedCallback([&inputStateComponent, &physicsComponent, e, playerEntity] {
            if (e == playerEntity && !inputStateComponent.forward) {
                physicsComponent.targetSpeed = 20.0f;
                inputStateComponent.forward = true;
            }
        });
        bufferedInput["Backward"].AddPressedCallback([&inputStateComponent, &physicsComponent, e, playerEntity] {
            if (e == playerEntity && !inputStateComponent.backward) {
                physicsComponent.targetSpeed = -20.0f;
                inputStateComponent.backward = true;
            }
        });
        bufferedInput["Left"].AddPressedCallback([&inputStateComponent, &transformComponent, &physicsComponent, e, playerEntity] {
            if (e == playerEntity && !inputStateComponent.left) {
                physicsComponent.rotationSpeed = 180.0f;
                inputStateComponent.left = true;
            }
        });
        bufferedInput["Right"].AddPressedCallback([&inputStateComponent, &physicsComponent, e, playerEntity] {
            if (e == playerEntity && !inputStateComponent.right) {
                physicsComponent.rotationSpeed = -180.0f;
                inputStateComponent.right = true;
            }
        });

        bufferedInput["Forward"].AddReleasedCallback([&physicsComponent, &inputStateComponent] {
            physicsComponent.targetSpeed = 0.0f;
            inputStateComponent.forward = false;
        });
        bufferedInput["Backward"].AddReleasedCallback([&physicsComponent, &inputStateComponent] {
            physicsComponent.targetSpeed = 0.0f;
            inputStateComponent.backward = false;
        });
        bufferedInput["Left"].AddReleasedCallback([&physicsComponent, &transformComponent, &inputStateComponent] {
            physicsComponent.rotationSpeed = 0.0f;
            inputStateComponent.left = false;
        });
        bufferedInput["Right"].AddReleasedCallback([&physicsComponent, &transformComponent, &inputStateComponent] {
            physicsComponent.rotationSpeed = 0.0f;
            inputStateComponent.right = false;
        });
    }
}

void UpdatePhysics(cs381::Scene<cs381::ComponentStorage>& scene, float dt) {
    if (!scene.HasComponent<PhysicsComponent>(0)) {
        return;
    }
    auto& transformComponent = scene.GetComponent<TransformComponent>(0);
    auto& physicsComponent = scene.GetComponent<PhysicsComponent>(0);
    
    transformComponent.heading += physicsComponent.rotationSpeed * dt;
    while (transformComponent.heading >= 360.0f) transformComponent.heading -= 360.0f;
    while (transformComponent.heading < 0.0f) transformComponent.heading += 360.0f;

    float radians = DEG2RAD * (transformComponent.heading + raylib::Degree(270));

    physicsComponent.speed = std::lerp(physicsComponent.speed, physicsComponent.targetSpeed, dt);
    physicsComponent.velocity = raylib::Vector3{ cos(radians) * physicsComponent.speed, 0.0f, -sin(radians) * physicsComponent.speed };

    transformComponent.position = transformComponent.position + physicsComponent.velocity * dt;
}

void UpdateCamera(raylib::Camera3D& camera, cs381::Scene<cs381::ComponentStorage>& scene) {
    if (!scene.HasComponent<TransformComponent>(0)) {
        return;
    }
    auto& transformComponent = scene.GetComponent<TransformComponent>(0);
    camera.position = transformComponent.position + raylib::Vector3{0.0f, 20.0f, -40.0f};
    camera.target = transformComponent.position;
}

void CheckCollision(cs381::Scene<cs381::ComponentStorage>& scene, BroccoliCounterState* broccoliState) {
    if (!scene.HasComponent<TransformComponent>(0)) {
        return;
    }
    if (!scene.HasComponent<TransformComponent>(1)) {
        return;
    }
    if (!scene.HasComponent<RenderComponent>(0)) {
        return;
    }
    if (!scene.HasComponent<RenderComponent>(1)) {
        return;
    }
    auto& transformComponentPlayer = scene.GetComponent<TransformComponent>(0);
    auto& transformComponentObject = scene.GetComponent<TransformComponent>(1);

    auto& renderComponentPlayer = scene.GetComponent<RenderComponent>(0);
    auto& renderComponentObject = scene.GetComponent<RenderComponent>(1);

    float radius = 3.0f;
    auto& playerPosition = transformComponentPlayer.position;
    auto& objectPosition = transformComponentObject.position;
    if (std::fabs(playerPosition.x - objectPosition.x) < radius && std::fabs(playerPosition.z - objectPosition.z) < radius) {
        float x = std::rand() % 200 - 100;
        float z = std::rand() % 200 - 100;
        objectPosition.x = x;
        objectPosition.z = z;
        transformComponentObject.position = objectPosition;
        broccoliState->broccoliCounter++;
    }

    if (playerPosition.x > 100) {
        playerPosition.x = 100;
    }
    if (playerPosition.x < -100) {
        playerPosition.x = -100;
    }
    if (playerPosition.z > 100) {
        playerPosition.z = 100;
    }
    if (playerPosition.z < -100) {
        playerPosition.z = -100;
    }
    transformComponentPlayer.position = playerPosition;
}

int main() {
    const int windowHeight = 700;
    const int windowWidth = 1000;
    std::string title = "CS381 - Assignment 9";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 20.0f, -40.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    raylib::Model grass = raylib::Mesh::Plane(200, 200, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.png");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    BroccoliCounterState broccoliState = InitBroccoliCounter();

    raylib::Model plant("../assets/Kenny-Furniture-Kit/plantSmall1.glb");
    raylib::Model broccoli("../assets/Kenny-Food-Kit/broccoli.glb");

    raylib::BufferedInput bufferedInput;
    bufferedInput["Forward"] = raylib::Action::key(KEY_W).move();
    bufferedInput["Backward"] = raylib::Action::key(KEY_S).move();
    bufferedInput["Left"] = raylib::Action::key(KEY_A).move();
    bufferedInput["Right"] = raylib::Action::key(KEY_D).move();

    cs381::Scene<cs381::ComponentStorage> scene;

    cs381::Entity plantEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(plantEntity);
    scene.GetComponent<RenderComponent>(plantEntity).model = &plant;
    scene.AddComponent<TransformComponent>(plantEntity);
    scene.GetComponent<TransformComponent>(plantEntity).position = raylib::Vector3{0.0f, 0.0f, 0.0f};
    scene.GetComponent<TransformComponent>(plantEntity).scale = 10;
    scene.AddComponent<InputStateComponent>(plantEntity);
    scene.AddComponent<PhysicsComponent>(plantEntity);

    cs381::Entity broccoliEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(broccoliEntity);
    scene.GetComponent<RenderComponent>(broccoliEntity).model = &broccoli;
    scene.AddComponent<TransformComponent>(broccoliEntity);
    scene.GetComponent<TransformComponent>(broccoliEntity).position = raylib::Vector3{10.0f, 0.0f, 0.0f};
    scene.GetComponent<TransformComponent>(broccoliEntity).scale = 10;

    while (!window.ShouldClose()) {
        bufferedInput.PollEvents();

        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
                grass.Draw({});
                RenderSystem(scene);
                if (broccoliState.startGame) {
                    InputSystem(scene, bufferedInput);
                    UpdatePhysics(scene, window.GetFrameTime());
                    UpdateCamera(camera, scene);
                    CheckCollision(scene, &broccoliState);
                }
            }
            camera.EndMode();
            GuiBroccoliCounter(&broccoliState);
        }
        window.EndDrawing();
    }

    return 0;
}