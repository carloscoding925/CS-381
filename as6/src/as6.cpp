#include <raylib-cpp.hpp>
#include <BufferedRaylib.hpp>
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
    bool drawBoundingBox = false;

    MeshRenderComponent(cs381::Entity& e, raylib::Model* model)
        : cs381::Component(e), model(model) {}

    void Tick(float dt) override {
        if (model == nullptr) { return; }

        if (drawBoundingBox) {
            DrawBoundedModel(*model, [this](raylib::Matrix& matrix){
                auto& transform = Object().Transform();
                return matrix.Translate(transform.position).RotateY(transform.heading);
            });
        }
        else {
            DrawModel(*model, [this](raylib::Matrix& matrix){
                auto& transform = Object().Transform();
                return matrix.Translate(transform.position).RotateY(transform.heading);
            });
        }
    }
};

struct PhysicsProperties {
    float maxSpeed;
    float acceleration;
    float turningRate;
};

struct PhysicsComponent : cs381::Component {
    raylib::Vector3 velocity;
    float speed;
    PhysicsProperties properties;

    PhysicsComponent(cs381::Entity& e, const PhysicsProperties& props)
        : cs381::Component(e), velocity{0, 0, 0}, speed(0), properties(props) {}

    void Tick(float dt) override {
        auto& transform = Object().Transform();
        float radians = DEG2RAD * transform.heading;
        velocity = raylib::Vector3{ cos(radians) * speed, 0, -sin(radians) * speed };
        transform.position = transform.position + velocity * dt;
    }

    void Accelerate(float amount) {
        speed += amount * properties.acceleration;
        if (speed > properties.maxSpeed) speed = properties.maxSpeed;
        if (speed < 0) speed = 0;
    }

    void Turn(float amount) {
        auto& transform = Object().Transform();
        transform.heading += amount * properties.turningRate;
    }
};

struct InputComponent : cs381::Component {
    raylib::BufferedInput* input;

    InputComponent(cs381::Entity& e, raylib::BufferedInput* input)
        : cs381::Component(e), input(input) {}

    void Tick(float dt) override {
        (*input)["Forward"].AddPressedCallback([this](){
            Object().GetComponent<PhysicsComponent>()->get().Accelerate(100);
        });

        (*input)["Backward"].AddPressedCallback([this](){
            Object().GetComponent<PhysicsComponent>()->get().Accelerate(-100);
        });

        (*input)["Left"].AddPressedCallback([this](){
            Object().GetComponent<PhysicsComponent>()->get().Turn(10);
        });

        (*input)["Right"].AddPressedCallback([this](){
            Object().GetComponent<PhysicsComponent>()->get().Turn(-10);
        });

        (*input)["Stop"].AddPressedCallback([this](){
            Object().GetComponent<PhysicsComponent>()->get().speed = 0;
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
    raylib::Model ambulance("../assets/Kenny Car Kit/ambulance.glb");
    raylib::Model garbageTruck("../assets/Kenny Car Kit/garbage-truck.glb");
    raylib::Model sportsSedan("../assets/Kenny Car Kit/sedan-sports.glb");
    raylib::Model fireTruck("../assets/Kenny Car Kit/firetruck.glb");

    PhysicsProperties carProperties[] = {
        { 100.0f, 10.0f, 45.0f }, // Truck
        { 120.0f, 12.0f, 50.0f }, // Ambulance
        { 80.0f, 8.0f, 40.0f },   // Garbage Truck
        { 150.0f, 15.0f, 60.0f }, // Sports Sedan
        { 90.0f, 9.0f, 45.0f }    // Fire Truck
    };
    
    PhysicsProperties rocketProperties = { 200.0f, 20.0f, 30.0f };

    rocket.transform = raylib::Matrix::Identity().Scale(30);
    truck.transform = raylib::Matrix::Identity().Scale(30).RotateY(raylib::Degree(90));
    ambulance.transform = raylib::Matrix::Identity().Scale(30).RotateY(raylib::Degree(90));
    garbageTruck.transform = raylib::Matrix::Identity().Scale(30).RotateY(raylib::Degree(90));
    sportsSedan.transform = raylib::Matrix::Identity().Scale(30).RotateY(raylib::Degree(90));
    fireTruck.transform = raylib::Matrix::Identity().Scale(30).RotateY(raylib::Degree(90));

    raylib::BufferedInput input;
    input["Forward"] = raylib::Action::key(KEY_W).move();
    input["Backward"] = raylib::Action::key(KEY_S).move();
    input["Left"] = raylib::Action::key(KEY_A).move();
    input["Right"] = raylib::Action::key(KEY_D).move();
    input["Stop"] = raylib::Action::key(KEY_SPACE).move();

    std::vector<cs381::Entity> entities;

    cs381::Entity& rocketEntity = entities.emplace_back();
    rocketEntity.AddComponent<MeshRenderComponent>(&rocket);
    rocketEntity.AddComponent<PhysicsComponent>(rocketProperties);
    rocketEntity.AddComponent<InputComponent>(&input);
    rocketEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{0, 0, 0};

    cs381::Entity& truckEntity = entities.emplace_back();
    truckEntity.AddComponent<MeshRenderComponent>(&truck);
    truckEntity.AddComponent<PhysicsComponent>(carProperties[0]);
    truckEntity.AddComponent<InputComponent>(&input);
    truckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{100, 0, 0};

    cs381::Entity& ambulanceEntity = entities.emplace_back();
    ambulanceEntity.AddComponent<MeshRenderComponent>(&ambulance);
    ambulanceEntity.AddComponent<PhysicsComponent>(carProperties[1]);
    ambulanceEntity.AddComponent<InputComponent>(&input);
    ambulanceEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-100, 0, 0};

    cs381::Entity& garbageTruckEntity = entities.emplace_back();
    garbageTruckEntity.AddComponent<MeshRenderComponent>(&garbageTruck);
    garbageTruckEntity.AddComponent<PhysicsComponent>(carProperties[2]);
    garbageTruckEntity.AddComponent<InputComponent>(&input);
    garbageTruckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{200, 0, 0};

    cs381::Entity& sportsSedanEntity = entities.emplace_back();
    sportsSedanEntity.AddComponent<MeshRenderComponent>(&sportsSedan);
    sportsSedanEntity.AddComponent<PhysicsComponent>(carProperties[3]);
    sportsSedanEntity.AddComponent<InputComponent>(&input);
    sportsSedanEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-200, 0, 0};

    cs381::Entity& fireTruckEntity = entities.emplace_back();
    fireTruckEntity.AddComponent<MeshRenderComponent>(&fireTruck);
    fireTruckEntity.AddComponent<PhysicsComponent>(carProperties[4]);
    fireTruckEntity.AddComponent<InputComponent>(&input);
    fireTruckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{300, 0, 0};

    size_t selectedEntity = 0;
    entities[selectedEntity].GetComponent<MeshRenderComponent>()->get().drawBoundingBox = true;

    while(!window.ShouldClose()) {
        input.PollEvents();

        window.BeginDrawing();
        {
            camera.BeginMode();
            {
                window.ClearBackground(raylib::Color::White());
                sky.Draw();
                grass.Draw({});

                auto dt = window.GetFrameTime();
                for(auto& e : entities) {
                    e.Tick(dt);
                }
            }
            camera.EndMode();
        }
        window.EndDrawing();

        if (raylib::Keyboard::IsKeyPressed(KEY_TAB)) {
            selectedEntity = (selectedEntity + 1) % entities.size();
            entities[selectedEntity].GetComponent<MeshRenderComponent>()->get().drawBoundingBox = true;

            for (size_t i = 0; i < entities.size(); i++) {
                if (i != selectedEntity) {
                    entities[i].GetComponent<MeshRenderComponent>()->get().drawBoundingBox = false;
                }
            }
        }
    }

    return 0;
}