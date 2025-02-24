#include "Color.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"
#include <iostream>
#include <raymath.h>

// Enforce that the type is a function which takes a raylib::Matrix and returns a raylib::Matrix.
template<typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

// Draws the model to the screen with the given transform applied. 
void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    auto box = model.GetTransformedBoundingBox();
    DrawBoundingBox(box, raylib::Color::Black());
    model.transform = backup;
}

int main() {
    // Window & window variables
    const int screenWidth = 1000;
    const int screenHeight = 700;
    const std::string title = "CS381 - Assignment 3";
    raylib::Window window(screenWidth, screenHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    // Truck & Wheel models
    raylib::Model truck("../assets/Kenny Car Kit/truck.glb");
    truck.transform = raylib::Matrix::Identity().Scale(5);
    raylib::Model wheels("../assets/Kenny Car Kit/wheel-default.glb");
    wheels.transform = raylib::Matrix::Identity().Scale(5);

    // Creating the grass plane texture
    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    // Creating sky and camera
    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 10.0f, 30.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    // Truck physics variables
    float targetSpeed = 0;
    float truckHeading = 0;
    float truckSpeed = 0;
    float truckYTarget = 0;
    raylib::Vector3 truckPosition = { 0.0f, 0.0f, 0.0f };

    // Timer & booleans for key press control
    float timer = 0;
    bool isLeftPressed = false;
    bool isRightPressed = false;

    // Main game loop
    while(!window.ShouldClose()) {

        /* Keys for controlling truck movement
        * W, S, and SPACE for controlling truck movement on the x-z plane
        * UP and DOWN for controlling truck movement on the y plane
        */
        if (raylib::Keyboard::IsKeyDown(KEY_W) && timer <= 0) {
            timer = 1.0;
            targetSpeed = 5.0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_S) && timer <= 0) {
            timer = 1.0;
            targetSpeed = 0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_SPACE) && timer <= 0) {
            timer = 1.0;
            targetSpeed = 0;
            truckSpeed = 0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_UP) && timer <= 0) {
            timer = 1.0;
            truckYTarget = 3.0;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_DOWN) && timer <= 0) {
            timer = 1.0;
            truckYTarget = 0.0;
        }
        timer -= window.GetFrameTime();
        
        /* Keys for controlling truck direction
        * A for turning the truck left
        * D for turning the truck right
        * Increments of 10 are used so the truck doesnt turn super sharp
        */
        if (raylib::Keyboard::IsKeyDown(KEY_A) && !isLeftPressed) {
            truckHeading = truckHeading + 10;
        }
        isLeftPressed = raylib::Keyboard::IsKeyDown(KEY_A);
        if (raylib::Keyboard::IsKeyDown(KEY_D) && !isRightPressed) {
            truckHeading = truckHeading - 10;
        }
        isRightPressed = raylib::Keyboard::IsKeyDown(KEY_D);

        // Truck Movement Physics - each line explained below

        // The sin and cos functions work better with radians, so we convert the truckHeading from degrees to rads
        float radians = DEG2RAD * truckHeading;
        // We want the truck speed to gradually increase like in real life so we use lerp to smooth the transition
        truckSpeed = std::lerp(truckSpeed, targetSpeed, window.GetFrameTime());
        // Here we calculate the direction vectors for the truck with cos being the X direction and sin being the Z direction
        // The Y direction is 0 since Y controls the up and down movement of the truck, and we implement this elsewhere. 
        raylib::Vector3 velocity = { cos(radians) * truckSpeed, 0, -sin(radians) * truckSpeed };
        // The truck position is calculated using the equation position = position + velocity * delta time
        truckPosition = truckPosition + velocity * window.GetFrameTime();

        // Here, when the user presses ARROW UP, truckYTarget is set to 3 and the Y position of the truck is gradually updated each frame until it reaches 3 or the player presses down.
        truckPosition.y = std::lerp(truckPosition.y, truckYTarget, window.GetFrameTime());

        /* This took me longer than i'd like to admit lol physics is hard.
        *
        * Essentially, we want the wheels to stay within the bounds of its respective wheel well.
        * If we ONLY rotate the wheels using RotateY(), the wheels stay in place and do not follow the truck, so we need to use the trucks position and heading to
        * dynamically position the wheels as the trucks heading changes.
        * From a starting position, the wheels will be at a certain distance from the trucks center (4.6, 0, -2.5) for the trucks front right wheel for example.
        * When the trucks heading changes, we can use sin() and cos() multiplied by the starting vector to determine the new position of the wheel as vectors, and then add these vectors
        * to determine the new position.
        */
        raylib::Vector3 frontRightWheelPos = truckPosition + raylib::Vector3{4.6f * cos(radians) + 2.5f * sin(radians), 0, 4.6f * -sin(radians) - 2.5f * -cos(radians)};
        raylib::Vector3 backRightWheelPos = truckPosition + raylib::Vector3{-3.8f * cos(radians) + 2.5f * sin(radians), 0, -3.8f * -sin(radians) - 2.5f * -cos(radians)};
        raylib::Vector3 frontLeftWheelPos = truckPosition + raylib::Vector3{4.6f * cos(radians) - 2.5f * sin(radians), 0, 4.6f * -sin(radians) + 2.5f * -cos(radians)};
        raylib::Vector3 backLeftWheelPos = truckPosition + raylib::Vector3{-3.8f * cos(radians) - 2.5f * sin(radians), 0, -3.8f * -sin(radians) + 2.5f * -cos(radians)};

        // We want the camera to follow the truck so we update the cameras position with the trucks position at each frame.
        camera.position = truckPosition + raylib::Vector3{0.0f, 10.0f, 30.0f};
        camera.target = truckPosition;

        // Draw the scene
        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::White());
            camera.BeginMode();
            {
                // Draw the environment
                sky.Draw();
                grass.Draw({});
                // Truck
                DrawBoundedModel(truck, [&truckPosition, &truckHeading](raylib::Transform t) {
                    return t.Translate(truckPosition).RotateY(raylib::Degree(truckHeading + 90));
                });
                // Front Right Wheel
                DrawBoundedModel(wheels, [&truckHeading, &frontRightWheelPos](raylib::Transform t) {
                    return t.Translate(frontRightWheelPos).RotateY(raylib::Degree(truckHeading + 270));
                });
                // Back Right Wheel
                DrawBoundedModel(wheels, [&truckHeading, &backRightWheelPos](raylib::Transform t) {
                    return t.Translate(backRightWheelPos).RotateY(raylib::Degree(truckHeading + 270));
                });
                // Front Left Wheel
                DrawBoundedModel(wheels, [&truckHeading, &frontLeftWheelPos](raylib::Transform t) {
                    return t.Translate(frontLeftWheelPos).RotateY(raylib::Degree(truckHeading + 90));
                });
                // Back Left Wheel
                DrawBoundedModel(wheels, [&truckHeading, &backLeftWheelPos](raylib::Transform t) {
                    return t.Translate(backLeftWheelPos).RotateY(raylib::Degree(truckHeading + 90));
                });
            }
            camera.EndMode();

        }
        window.EndDrawing();

    }

    return 0;
}