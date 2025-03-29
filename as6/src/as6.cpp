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
    bool drawBoundingBox = false;
    bool isRocket = false;

    MeshRenderComponent(cs381::Entity& e, raylib::Model* model)
        : cs381::Component(e), model(model) {}

    void Tick(float dt) override {
        if (model == nullptr) { return; }

        if (isRocket) {
            if (drawBoundingBox) {
                DrawBoundedModel(*model, [this](raylib::Matrix& matrix){
                    auto& transform = Object().Transform();
                    return matrix.Translate(transform.position).RotateZ(transform.heading * raylib::Degree(-1));
                });
            }
            else {
                DrawModel(*model, [this](raylib::Matrix& matrix){
                    auto& transform = Object().Transform();
                    return matrix.Translate(transform.position).RotateZ(transform.heading * raylib::Degree(-1));
                });
            }
        }
        else {
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
    }
};

struct PhysicsProperties {
    float maxSpeed;
    float acceleration;
    float turningSpeed;
};

struct PhysicsComponent : cs381::Component {
    PhysicsProperties properties;
    float speed = 0;
    float maxSpeed = 0;

    PhysicsComponent(cs381::Entity& e, const PhysicsProperties& props)
        : cs381::Component(e), properties(props) {}

    void Tick(float dt) override {
        bool isRocket = Object().GetComponent<MeshRenderComponent>()->get().isRocket;
        if (isRocket) {
            auto& transform = Object().Transform();
            float radians = DEG2RAD * (Object().Transform().heading + raylib::Degree(90));
            speed = std::lerp(speed, maxSpeed, dt * properties.acceleration);
            raylib::Vector3 velocity = { -cos(radians) * speed, sin(radians) * speed, 0 };
            transform.position = transform.position + velocity * dt;
        }
        else {
            auto& transform = Object().Transform();
            float radians = DEG2RAD * Object().Transform().heading;
            speed = std::lerp(speed, maxSpeed, dt * properties.acceleration);
            raylib::Vector3 velocity = { cos(radians) * speed, 0, -sin(radians) * speed };
            transform.position = transform.position + velocity * dt; 
        }
    }

    void Accelerate() {
        maxSpeed = properties.maxSpeed;
    }

    void SlowDown() {
        maxSpeed = 0;
    }

    void Stop() {
        maxSpeed = 0;
        speed = 0;
    }

    void TurnLeft(float dt) {
        auto& transform = Object().Transform();
        transform.heading = transform.heading + raylib::Degree(dt * properties.turningSpeed);

        if (transform.heading > 360) {
            transform.heading = transform.heading - raylib::Degree(360);
        }
        else if (transform.heading < -360) {
            transform.heading = transform.heading + raylib::Degree(360);
        }
    }

    void TurnRight(float dt) {
        auto& transform = Object().Transform();
        transform.heading = transform.heading - raylib::Degree(dt * properties.turningSpeed);
        
        if (transform.heading > 360) {
            transform.heading = transform.heading - raylib::Degree(360);
        }
        else if (transform.heading < 0) {
            transform.heading = transform.heading + raylib::Degree(360);
        }
    } 
};

struct InputComponent : cs381::Component {
    raylib::BufferedInput* input;
    size_t* selectedEntityIndex;
    size_t entityIndex;

    InputComponent(cs381::Entity& e, raylib::BufferedInput* input, size_t* selectedEntityIndex, size_t entityIndex)
        : cs381::Component(e), input(input), selectedEntityIndex(selectedEntityIndex), entityIndex(entityIndex) {}

    void Tick(float dt) override {
        (*input)["Forward"].AddPressedCallback([this](){
            if (*selectedEntityIndex == entityIndex) {
                Object().GetComponent<PhysicsComponent>()->get().Accelerate();
            }
        });

        (*input)["Backward"].AddPressedCallback([this](){
            if (*selectedEntityIndex == entityIndex) {
                Object().GetComponent<PhysicsComponent>()->get().SlowDown();
            }
        });

        (*input)["Left"].AddPressedCallback([this, dt](){
            if (*selectedEntityIndex == entityIndex) {
                Object().GetComponent<PhysicsComponent>()->get().TurnLeft(dt);
            }
        });

        (*input)["Right"].AddPressedCallback([this, dt](){
            if (*selectedEntityIndex == entityIndex) {
                Object().GetComponent<PhysicsComponent>()->get().TurnRight(dt);
            }
        });

        (*input)["Stop"].AddPressedCallback([this](){
            if (*selectedEntityIndex == entityIndex) {
                Object().GetComponent<PhysicsComponent>()->get().Stop();
            }
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

    raylib::AudioDevice audioDevice;
    raylib::Music wind = LoadMusicStream("../assets/audio/wind.mp3");
    wind.SetLooping(true);
    PlayMusicStream(wind);

    raylib::Model rocket("../assets/Kenny Space Kit/rocketA.glb");
    raylib::Model truck("../assets/Kenny Car Kit/truck.glb");
    raylib::Model ambulance("../assets/Kenny Car Kit/ambulance.glb");
    raylib::Model garbageTruck("../assets/Kenny Car Kit/garbage-truck.glb");
    raylib::Model sportsSedan("../assets/Kenny Car Kit/sedan-sports.glb");
    raylib::Model fireTruck("../assets/Kenny Car Kit/firetruck.glb");

    PhysicsProperties properties[] = {
        { 10.0f, 2.0f, 1.0f }, // Rocket
        { 40.0f, 2.0f, 1.5f }, // Truck
        { 60.0f, 1.0f, 1.0f }, // Ambulance
        { 20.0f, 0.5f, 0.5f }, // Garbage Truck
        { 100.0f, 2.0f, 2.0f }, // Sports Sedan
        { 80.0f, 0.5f, 1.0f }, // Fire Truck
    };

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
    size_t selectedEntity = 0;

    cs381::Entity& rocketEntity = entities.emplace_back();
    rocketEntity.AddComponent<MeshRenderComponent>(&rocket);
    rocketEntity.AddComponent<PhysicsComponent>(properties[0]);
    rocketEntity.AddComponent<InputComponent>(&input, &selectedEntity, 0);
    rocketEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{0, 0, 0};
    rocketEntity.GetComponent<MeshRenderComponent>()->get().isRocket = true;

    cs381::Entity& truckEntity = entities.emplace_back();
    truckEntity.AddComponent<MeshRenderComponent>(&truck);
    truckEntity.AddComponent<PhysicsComponent>(properties[1]);
    truckEntity.AddComponent<InputComponent>(&input, &selectedEntity, 1);
    truckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{100, 0, 0};

    cs381::Entity& ambulanceEntity = entities.emplace_back();
    ambulanceEntity.AddComponent<MeshRenderComponent>(&ambulance);
    ambulanceEntity.AddComponent<PhysicsComponent>(properties[2]);
    ambulanceEntity.AddComponent<InputComponent>(&input, &selectedEntity, 2);
    ambulanceEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-100, 0, 0};

    cs381::Entity& garbageTruckEntity = entities.emplace_back();
    garbageTruckEntity.AddComponent<MeshRenderComponent>(&garbageTruck);
    garbageTruckEntity.AddComponent<PhysicsComponent>(properties[3]);
    garbageTruckEntity.AddComponent<InputComponent>(&input, &selectedEntity, 3);
    garbageTruckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{200, 0, 0};

    cs381::Entity& sportsSedanEntity = entities.emplace_back();
    sportsSedanEntity.AddComponent<MeshRenderComponent>(&sportsSedan);
    sportsSedanEntity.AddComponent<PhysicsComponent>(properties[4]);
    sportsSedanEntity.AddComponent<InputComponent>(&input, &selectedEntity, 4);
    sportsSedanEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{-200, 0, 0};

    cs381::Entity& fireTruckEntity = entities.emplace_back();
    fireTruckEntity.AddComponent<MeshRenderComponent>(&fireTruck);
    fireTruckEntity.AddComponent<PhysicsComponent>(properties[5]);
    fireTruckEntity.AddComponent<InputComponent>(&input, &selectedEntity, 5);
    fireTruckEntity.GetComponent<cs381::TransformComponent>()->get().position = raylib::Vector3{300, 0, 0};

    entities[selectedEntity].GetComponent<MeshRenderComponent>()->get().drawBoundingBox = true;

    while(!window.ShouldClose()) {
        input.PollEvents();
        SetMusicVolume(wind, 0.5f);
        UpdateMusicStream(wind);

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

    UnloadMusicStream(wind);
    audioDevice.Close();

    return 0;
}