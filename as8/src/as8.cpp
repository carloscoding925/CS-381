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
    bool drawBoundingBox = false;
};

struct TransformComponent {
    raylib::Vector3 position = {0.0f, 0.0f, 0.0f};
    raylib::Degree heading = 0;
};

struct KinematicPhysicsComponent {
    raylib::Vector3 velocity = {0.0f, 0.0f, 0.0f};
    float speed = 0.0f;
    float targetSpeed = 0.0f;
};

struct TwoDeePhysicsComponent {
    raylib::Degree targetHeading = 0;
    float maxSpeed = 100.0f;
    float acceleration = 5.0f;
    float turningRate = 5.0f;
};

struct ThreeDeeComponent {
    float maxSpeed = 100.0f;
    float acceleration = 5.0f;
    float turningRate = 5.0f;  
};

void DrawAllModels(cs381::Scene<cs381::ComponentStorage>& scene, int selectedEntity)
{
    for (cs381::Entity e = 0; e < scene.entityMasks.size(); e++)
    {
        if(!scene.HasComponent<RenderComponent>(e)) continue;
		if(!scene.HasComponent<TransformComponent>(e)) continue;

        auto& rendering = scene.GetComponent<RenderComponent>(e);
		auto& transform = scene.GetComponent<TransformComponent>(e);

		if(scene.HasComponent<KinematicPhysicsComponent>(e))
		{
			if(selectedEntity == e)
			{
				DrawBoundedModel(*rendering.model, [&transform](const raylib::Transform& m) -> raylib::Transform {
					return m.Translate(transform.position).Scale(30);
				});
			}
			else
			{
				DrawModel(*rendering.model, [&transform](const raylib::Transform& m) -> raylib::Transform {
					return m.Translate(transform.position).Scale(30);
				});
			}

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
    raylib::Camera3D camera(raylib::Vector3{0.0f, 120.0f, -500.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    raylib::Model rocket("../assets/Kenny Space Kit/rocketA.glb");
    raylib::Model truck("../assets/Kenny Car Kit/truck.glb");
    raylib::Model ambulance("../assets/Kenny Car Kit/ambulance.glb");
    raylib::Model garbageTruck("../assets/Kenny Car Kit/garbage-truck.glb");
    raylib::Model sportsSedan("../assets/Kenny Car Kit/sedan-sports.glb");
    raylib::Model fireTruck("../assets/Kenny Car Kit/firetruck.glb");

    int selectedEntity = 0;
    raylib::BufferedInput bufferedInput;

    bufferedInput["Forward"] = raylib::Action::key(KEY_W).move();
    bufferedInput["SlowDown"] = raylib::Action::key(KEY_S).move();
    bufferedInput["Left"] = raylib::Action::key(KEY_A).move();
    bufferedInput["Right"] = raylib::Action::key(KEY_D).move();
    bufferedInput["Stop"] = raylib::Action::key(KEY_SPACE).move();
    bufferedInput["Switch"] = raylib::Action::key(KEY_TAB).SetPressedCallback([&selectedEntity]{
        if (selectedEntity < 6) {
            selectedEntity++;
        } else {
            selectedEntity = 0;
        }
    }).move();

    cs381::Scene<cs381::ComponentStorage> scene;
    
    cs381::Entity rocketEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(rocketEntity);
    scene.GetComponent<RenderComponent>(rocketEntity).model = &rocket;
    scene.AddComponent<TransformComponent>(rocketEntity);
    scene.AddComponent<KinematicPhysicsComponent>(rocketEntity);
    scene.AddComponent<TwoDeePhysicsComponent>(rocketEntity);
    scene.AddComponent<ThreeDeeComponent>(rocketEntity);
    
    cs381::Entity truckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(truckEntity);
    scene.GetComponent<RenderComponent>(truckEntity).model = &truck;
    scene.AddComponent<TransformComponent>(truckEntity);
    scene.AddComponent<KinematicPhysicsComponent>(truckEntity);
    scene.AddComponent<TwoDeePhysicsComponent>(truckEntity);

    cs381::Entity ambulanceEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(ambulanceEntity);
    scene.GetComponent<RenderComponent>(ambulanceEntity).model = &ambulance;
    scene.AddComponent<TransformComponent>(ambulanceEntity);
    scene.AddComponent<KinematicPhysicsComponent>(ambulanceEntity);
    scene.AddComponent<TwoDeePhysicsComponent>(ambulanceEntity);

    cs381::Entity garbageTruckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(garbageTruckEntity);
    scene.GetComponent<RenderComponent>(garbageTruckEntity).model = &garbageTruck;
    scene.AddComponent<TransformComponent>(garbageTruckEntity);
    scene.AddComponent<KinematicPhysicsComponent>(garbageTruckEntity);
    scene.AddComponent<TwoDeePhysicsComponent>(garbageTruckEntity);

    cs381::Entity sportsSedanEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(sportsSedanEntity);
    scene.GetComponent<RenderComponent>(sportsSedanEntity).model = &sportsSedan;
    scene.AddComponent<TransformComponent>(sportsSedanEntity);
    scene.AddComponent<KinematicPhysicsComponent>(sportsSedanEntity);
    scene.AddComponent<TwoDeePhysicsComponent>(sportsSedanEntity);

    cs381::Entity fireTruckEntity = scene.CreateEntity();
    scene.AddComponent<RenderComponent>(fireTruckEntity);
    scene.GetComponent<RenderComponent>(fireTruckEntity).model = &fireTruck;
    scene.AddComponent<TransformComponent>(fireTruckEntity);
    scene.AddComponent<KinematicPhysicsComponent>(fireTruckEntity);
    scene.AddComponent<TwoDeePhysicsComponent>(fireTruckEntity);

    while(!window.ShouldClose()) {
        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
                DrawAllModels(scene, selectedEntity);
            }
            camera.EndMode();
        }
        window.EndDrawing();
    }

    return 0;
}