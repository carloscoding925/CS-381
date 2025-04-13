#include <raylib-cpp.hpp>
#include <BufferedRaylib.hpp>
#include "Window.hpp"
#include "../assets/skybox.hpp"
#include <iostream>
#include "ECS.hpp"

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
    raylib::Model* model;
};

struct TransformComponent {
    raylib::Vector3 position = {0.0f, 0.0f, 0.0f};
    raylib::Degree heading = 0;
};

struct PhysicsProperties {
    float speedIncrement = 0.0f;
    float acceleration = 0.0f;
    float turningRate = 0.0f;  
};

struct KinematicsComponent {
    raylib::Vector3 velocity = {0.0f, 0.0f, 0.0f};
    float speed = 0.0f;
    float targetSpeed = 0.0f;
};

struct Physics2DComponent {
    raylib::Degree targetHeading = 0;
};

struct Physics3DComponent {
    raylib::Quaternion targetRotation = {0.0f, 0.0f, 0.0f, 1.0f};
};

struct InputStateComponent {
    bool forward = false;
    bool slowDown = false;
    bool left = false;
    bool right = false;
};

void RenderEntities(cs381::Scene<cs381::ComponentStorage>& scene, int selectedEntity) {
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<RenderComponent>(e)) {
            continue;
        }
        if (!scene.HasComponent<TransformComponent>(e)) {
            continue;
        }

        auto& renderComponent = scene.GetComponent<RenderComponent>(e);
        auto& transformComponent = scene.GetComponent<TransformComponent>(e);

        if (selectedEntity == e) {
            DrawBoundedModel(*renderComponent.model, [&transformComponent](const raylib::Transform& m) -> raylib::Transform {
                return m.Translate(transformComponent.position).Scale(30).RotateY(transformComponent.heading + raylib::Degree(90));
            });
        } 
        else {
            DrawModel(*renderComponent.model, [&transformComponent](const raylib::Transform& m) -> raylib::Transform {
                return m.Translate(transformComponent.position).Scale(30).RotateY(transformComponent.heading + raylib::Degree(90));
            });
        }
    }
}

void Update2DPhysics(cs381::Scene<cs381::ComponentStorage>& scene, float dt) {
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<TransformComponent>(e)) continue;
        if (!scene.HasComponent<KinematicsComponent>(e)) continue;

        auto& transformComponent = scene.GetComponent<TransformComponent>(e);
        auto& kinematicsComponent = scene.GetComponent<KinematicsComponent>(e);

        if (scene.HasComponent<Physics2DComponent>(e)) {
            auto& physicsComponent = scene.GetComponent<Physics2DComponent>(e);

            if (!scene.HasComponent<PhysicsProperties>(e)) {
                continue;
            }
            auto& physicsProperties = scene.GetComponent<PhysicsProperties>(e);

            transformComponent.heading = std::lerp(transformComponent.heading, physicsComponent.targetHeading, dt);
            float radians = DEG2RAD * transformComponent.heading;

            kinematicsComponent.speed = std::lerp(kinematicsComponent.speed, kinematicsComponent.targetSpeed, dt * physicsProperties.acceleration);
            kinematicsComponent.velocity = raylib::Vector3{ cos(radians) * kinematicsComponent.speed, 0.0f, -sin(radians) * kinematicsComponent.speed };

            transformComponent.position = transformComponent.position + kinematicsComponent.velocity * dt;
        }
        else if (scene.HasComponent<Physics3DComponent>(e)) {

        }
    }
}

void Update3DPhysics(cs381::Scene<cs381::ComponentStorage>& scene, float dt) {
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<TransformComponent>(e)) continue;
        if (!scene.HasComponent<KinematicsComponent>(e)) continue;

        auto& transformComponent = scene.GetComponent<TransformComponent>(e);
        auto& kinematicsComponent = scene.GetComponent<KinematicsComponent>(e);

        transformComponent.position = transformComponent.position + kinematicsComponent.velocity * dt;

        if (scene.HasComponent<Physics3DComponent>(e)) {
            kinematicsComponent.velocity = raylib::Vector3{kinematicsComponent.speed * -cos(transformComponent.heading), kinematicsComponent.speed * sin(transformComponent.heading), 0.0f};
        }
    }

}

void InputSystem(cs381::Scene<cs381::ComponentStorage>& scene, raylib::BufferedInput& bufferedInput, int& selectedEntity) {
    bool tabPressed = false;

    bufferedInput["Switch"].AddPressedCallback([&selectedEntity, &tabPressed]{
        if (!tabPressed) {
            selectedEntity = (selectedEntity + 1) % 6;
            tabPressed = true;
        }
    });
    bufferedInput["Switch"].AddReleasedCallback([&tabPressed]{
        tabPressed = false;
    });

    for (cs381::Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<TransformComponent>(e)) {
            continue;
        }
        if (!scene.HasComponent<KinematicsComponent>(e)) {
            continue;
        }

        auto& transformComponent = scene.GetComponent<TransformComponent>(e);
        auto& kinematicsComponent = scene.GetComponent<KinematicsComponent>(e);
        auto& inputStateComponent = scene.GetComponent<InputStateComponent>(e);

        if (scene.HasComponent<Physics2DComponent>(e)) {
            if (!scene.HasComponent<PhysicsProperties>(e)) {
                continue;
            }
            auto& physicsProperties = scene.GetComponent<PhysicsProperties>(e);
            auto& physicsComponent = scene.GetComponent<Physics2DComponent>(e);

            bufferedInput["Forward"].AddPressedCallback([&physicsProperties, &kinematicsComponent, &selectedEntity, e, &inputStateComponent]{
                if (selectedEntity == e && !inputStateComponent.forward) {
                    kinematicsComponent.targetSpeed = physicsProperties.speedIncrement + kinematicsComponent.targetSpeed;
                    inputStateComponent.forward = true;
                }
            });
            bufferedInput["SlowDown"].AddPressedCallback([&physicsProperties, &kinematicsComponent, &selectedEntity, e, &inputStateComponent]{
                if (selectedEntity == e && !inputStateComponent.slowDown) {
                    kinematicsComponent.targetSpeed = kinematicsComponent.targetSpeed - physicsProperties.speedIncrement;
                    inputStateComponent.slowDown = true;
                }
            });
            bufferedInput["Stop"].AddPressedCallback([&kinematicsComponent, &selectedEntity, e]{
                if (selectedEntity == e) {
                    kinematicsComponent.targetSpeed = 0.0f;
                }
            });
            bufferedInput["Left"].AddPressedCallback([&physicsComponent, &physicsProperties, &kinematicsComponent, &selectedEntity, e, &inputStateComponent]{
                if (selectedEntity == e && !inputStateComponent.left) {
                    physicsComponent.targetHeading = physicsComponent.targetHeading + raylib::Degree(physicsProperties.turningRate);
                    inputStateComponent.left = true;
                }
            });
            bufferedInput["Right"].AddPressedCallback([&physicsComponent, &physicsProperties, &kinematicsComponent, &selectedEntity, e, &inputStateComponent]{
                if (selectedEntity == e && !inputStateComponent.right) {
                    physicsComponent.targetHeading = physicsComponent.targetHeading - raylib::Degree(physicsProperties.turningRate);
                    inputStateComponent.right = true;
                }
            });

            bufferedInput["Forward"].AddReleasedCallback([&inputStateComponent]{
                inputStateComponent.forward = false;
            });
            bufferedInput["SlowDown"].AddReleasedCallback([&inputStateComponent]{
                inputStateComponent.slowDown = false;
            });
            bufferedInput["Left"].AddReleasedCallback([&inputStateComponent]{
                inputStateComponent.left = false;
            });
            bufferedInput["Right"].AddReleasedCallback([&inputStateComponent]{
                inputStateComponent.right = false;
            });
        }
        else if (scene.HasComponent<Physics3DComponent>(e)) {

        }
    }
}

int main() {
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "CS 381 - Assignment 8";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 180.0f, -600.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    raylib::Model grass = raylib::Mesh::Plane(1000, 1000, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.png");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    raylib::Model rocket("../assets/Kenny Space Kit/rocketA.glb");
    raylib::Model truck("../assets/Kenny Car Kit/truck.glb");
    raylib::Model ambulance("../assets/Kenny Car Kit/ambulance.glb");
    raylib::Model garbageTruck("../assets/Kenny Car Kit/garbage-truck.glb");
    raylib::Model sportsSedan("../assets/Kenny Car Kit/sedan-sports.glb");
    raylib::Model fireTruck("../assets/Kenny Car Kit/firetruck.glb");

    int selectedEntity = 0;
    raylib::BufferedInput bufferedInput;

    bufferedInput["Switch"] = raylib::Action::key(KEY_TAB).move();
    bufferedInput["Forward"] = raylib::Action::key(KEY_W).move();
    bufferedInput["SlowDown"] = raylib::Action::key(KEY_S).move();
    bufferedInput["Stop"] = raylib::Action::key(KEY_SPACE).move();
    bufferedInput["Left"] = raylib::Action::key(KEY_A).move();
    bufferedInput["Right"] = raylib::Action::key(KEY_D).move();

    cs381::Scene<cs381::ComponentStorage> scene;

    cs381::Entity rocketEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(rocketEntity);
    scene.GetComponent<RenderComponent>(rocketEntity).model = &rocket;
    scene.AddComponent<TransformComponent>(rocketEntity);
    scene.AddComponent<InputStateComponent>(rocketEntity);

    cs381::Entity truckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(truckEntity);
    scene.GetComponent<RenderComponent>(truckEntity).model = &truck;
    scene.AddComponent<TransformComponent>(truckEntity);
    scene.GetComponent<TransformComponent>(truckEntity).position = raylib::Vector3{100.0f, 0.0f, 0.0f};
    scene.AddComponent<PhysicsProperties>(truckEntity);
    scene.GetComponent<PhysicsProperties>(truckEntity).speedIncrement = 10.0f;
    scene.GetComponent<PhysicsProperties>(truckEntity).acceleration = 1.0f;
    scene.GetComponent<PhysicsProperties>(truckEntity).turningRate = 25.0f;
    scene.AddComponent<KinematicsComponent>(truckEntity);
    scene.AddComponent<Physics2DComponent>(truckEntity);
    scene.AddComponent<InputStateComponent>(truckEntity);

    cs381::Entity ambulanceEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(ambulanceEntity);
    scene.GetComponent<RenderComponent>(ambulanceEntity).model = &ambulance;
    scene.AddComponent<TransformComponent>(ambulanceEntity);
    scene.GetComponent<TransformComponent>(ambulanceEntity).position = raylib::Vector3{-100.0f, 0.0f, 0.0f};
    scene.AddComponent<PhysicsProperties>(ambulanceEntity);
    scene.GetComponent<PhysicsProperties>(ambulanceEntity).speedIncrement = 15.0f;
    scene.GetComponent<PhysicsProperties>(ambulanceEntity).acceleration = 2.0f;
    scene.GetComponent<PhysicsProperties>(ambulanceEntity).turningRate = 15.0f;
    scene.AddComponent<KinematicsComponent>(ambulanceEntity);
    scene.AddComponent<Physics2DComponent>(ambulanceEntity);
    scene.AddComponent<InputStateComponent>(ambulanceEntity);

    cs381::Entity garbageTruckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(garbageTruckEntity);
    scene.GetComponent<RenderComponent>(garbageTruckEntity).model = &garbageTruck;
    scene.AddComponent<TransformComponent>(garbageTruckEntity);
    scene.GetComponent<TransformComponent>(garbageTruckEntity).position = raylib::Vector3{200.0f, 0.0f, 0.0f};
    scene.AddComponent<PhysicsProperties>(garbageTruckEntity);
    scene.GetComponent<PhysicsProperties>(garbageTruckEntity).speedIncrement = 5.0f;
    scene.GetComponent<PhysicsProperties>(garbageTruckEntity).acceleration = 0.5f;
    scene.GetComponent<PhysicsProperties>(garbageTruckEntity).turningRate = 10.0f;
    scene.AddComponent<KinematicsComponent>(garbageTruckEntity);
    scene.AddComponent<Physics2DComponent>(garbageTruckEntity);
    scene.AddComponent<InputStateComponent>(garbageTruckEntity);

    cs381::Entity sportsSedanEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(sportsSedanEntity);
    scene.GetComponent<RenderComponent>(sportsSedanEntity).model = &sportsSedan;
    scene.AddComponent<TransformComponent>(sportsSedanEntity);
    scene.GetComponent<TransformComponent>(sportsSedanEntity).position = raylib::Vector3{-200.0f, 0.0f, 0.0f};
    scene.AddComponent<PhysicsProperties>(sportsSedanEntity);
    scene.GetComponent<PhysicsProperties>(sportsSedanEntity).speedIncrement = 25.0f;
    scene.GetComponent<PhysicsProperties>(sportsSedanEntity).acceleration = 2.0f;
    scene.GetComponent<PhysicsProperties>(sportsSedanEntity).turningRate = 30.0f;
    scene.AddComponent<KinematicsComponent>(sportsSedanEntity);
    scene.AddComponent<Physics2DComponent>(sportsSedanEntity);
    scene.AddComponent<InputStateComponent>(sportsSedanEntity);

    cs381::Entity fireTruckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(fireTruckEntity);
    scene.GetComponent<RenderComponent>(fireTruckEntity).model = &fireTruck;
    scene.AddComponent<TransformComponent>(fireTruckEntity);
    scene.GetComponent<TransformComponent>(fireTruckEntity).position = raylib::Vector3{300.0f, 0.0f, 0.0f};
    scene.AddComponent<PhysicsProperties>(fireTruckEntity);
    scene.GetComponent<PhysicsProperties>(fireTruckEntity).speedIncrement = 20.0f;
    scene.GetComponent<PhysicsProperties>(fireTruckEntity).acceleration = 1.5f;
    scene.GetComponent<PhysicsProperties>(fireTruckEntity).turningRate = 30.0f;
    scene.AddComponent<KinematicsComponent>(fireTruckEntity);
    scene.AddComponent<Physics2DComponent>(fireTruckEntity);
    scene.AddComponent<InputStateComponent>(fireTruckEntity);

    while(!window.ShouldClose()) {
        bufferedInput.PollEvents();

        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
                grass.Draw({});

                RenderEntities(scene, selectedEntity);
                Update2DPhysics(scene, window.GetFrameTime());
                Update3DPhysics(scene, window.GetFrameTime());
                InputSystem(scene, bufferedInput, selectedEntity);
            }
            camera.EndMode();
        }
        window.EndDrawing();
    }

    return 0;
}