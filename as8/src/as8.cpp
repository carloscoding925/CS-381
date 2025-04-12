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
    float maxSpeed = 0.0f;
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
                return m.Translate(transformComponent.position).Scale(30).RotateY(transformComponent.heading);
            });
        } 
        else {
            DrawModel(*renderComponent.model, [&transformComponent](const raylib::Transform& m) -> raylib::Transform {
                return m.Translate(transformComponent.position).Scale(30).RotateY(transformComponent.heading);
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

        transformComponent.position = transformComponent.position + kinematicsComponent.velocity * dt;

        if (scene.HasComponent<Physics2DComponent>(e)) {
            kinematicsComponent.velocity = raylib::Vector3{kinematicsComponent.speed * cos(transformComponent.heading), 0.0f, kinematicsComponent.speed * -sin(transformComponent.heading)};
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

    bufferedInput["Switch"].AddPressedCallback([&selectedEntity]{
        selectedEntity = (selectedEntity + 1) % 6;
    });

    cs381::Scene<cs381::ComponentStorage> scene;

    cs381::Entity rocketEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(rocketEntity);
    scene.GetComponent<RenderComponent>(rocketEntity).model = &rocket;
    scene.AddComponent<TransformComponent>(rocketEntity);

    cs381::Entity truckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(truckEntity);
    scene.GetComponent<RenderComponent>(truckEntity).model = &truck;
    scene.AddComponent<TransformComponent>(truckEntity);
    scene.GetComponent<TransformComponent>(truckEntity).position = raylib::Vector3{100.0f, 0.0f, 0.0f};
    scene.GetComponent<TransformComponent>(truckEntity).heading = 90;
    scene.AddComponent<PhysicsProperties>(truckEntity);
    scene.GetComponent<PhysicsProperties>(truckEntity).maxSpeed = 10.0f;
    scene.GetComponent<PhysicsProperties>(truckEntity).acceleration = 5.0f;
    scene.GetComponent<PhysicsProperties>(truckEntity).turningRate = 5.0f;
    scene.AddComponent<KinematicsComponent>(truckEntity);

    cs381::Entity ambulanceEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(ambulanceEntity);
    scene.GetComponent<RenderComponent>(ambulanceEntity).model = &ambulance;
    scene.AddComponent<TransformComponent>(ambulanceEntity);
    scene.GetComponent<TransformComponent>(ambulanceEntity).position = raylib::Vector3{-100.0f, 0.0f, 0.0f};
    scene.GetComponent<TransformComponent>(ambulanceEntity).heading = 90;
    scene.AddComponent<PhysicsProperties>(ambulanceEntity);
    scene.GetComponent<PhysicsProperties>(ambulanceEntity).maxSpeed = 15.0f;
    scene.GetComponent<PhysicsProperties>(ambulanceEntity).acceleration = 7.0f;
    scene.GetComponent<PhysicsProperties>(ambulanceEntity).turningRate = 10.0f;
    scene.AddComponent<KinematicsComponent>(ambulanceEntity);

    cs381::Entity garbageTruckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(garbageTruckEntity);
    scene.GetComponent<RenderComponent>(garbageTruckEntity).model = &garbageTruck;
    scene.AddComponent<TransformComponent>(garbageTruckEntity);
    scene.GetComponent<TransformComponent>(garbageTruckEntity).position = raylib::Vector3{200.0f, 0.0f, 0.0f};
    scene.GetComponent<TransformComponent>(garbageTruckEntity).heading = 90;
    scene.AddComponent<PhysicsProperties>(garbageTruckEntity);
    scene.GetComponent<PhysicsProperties>(garbageTruckEntity).maxSpeed = 8.0f;
    scene.GetComponent<PhysicsProperties>(garbageTruckEntity).acceleration = 4.0f;
    scene.GetComponent<PhysicsProperties>(garbageTruckEntity).turningRate = 3.0f;
    scene.AddComponent<KinematicsComponent>(garbageTruckEntity);

    cs381::Entity sportsSedanEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(sportsSedanEntity);
    scene.GetComponent<RenderComponent>(sportsSedanEntity).model = &sportsSedan;
    scene.AddComponent<TransformComponent>(sportsSedanEntity);
    scene.GetComponent<TransformComponent>(sportsSedanEntity).position = raylib::Vector3{-200.0f, 0.0f, 0.0f};
    scene.GetComponent<TransformComponent>(sportsSedanEntity).heading = 90;
    scene.AddComponent<PhysicsProperties>(sportsSedanEntity);
    scene.GetComponent<PhysicsProperties>(sportsSedanEntity).maxSpeed = 20.0f;
    scene.GetComponent<PhysicsProperties>(sportsSedanEntity).acceleration = 10.0f;
    scene.GetComponent<PhysicsProperties>(sportsSedanEntity).turningRate = 15.0f;
    scene.AddComponent<KinematicsComponent>(sportsSedanEntity);

    cs381::Entity fireTruckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(fireTruckEntity);
    scene.GetComponent<RenderComponent>(fireTruckEntity).model = &fireTruck;
    scene.AddComponent<TransformComponent>(fireTruckEntity);
    scene.GetComponent<TransformComponent>(fireTruckEntity).position = raylib::Vector3{300.0f, 0.0f, 0.0f};
    scene.GetComponent<TransformComponent>(fireTruckEntity).heading = 90;
    scene.AddComponent<PhysicsProperties>(fireTruckEntity);
    scene.GetComponent<PhysicsProperties>(fireTruckEntity).maxSpeed = 12.0f;
    scene.GetComponent<PhysicsProperties>(fireTruckEntity).acceleration = 6.0f;
    scene.GetComponent<PhysicsProperties>(fireTruckEntity).turningRate = 8.0f;
    scene.AddComponent<KinematicsComponent>(fireTruckEntity);

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
            }
            camera.EndMode();
        }
        window.EndDrawing();
    }

    return 0;
}