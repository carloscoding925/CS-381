#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"

// Ensure that the type is a function which takes a raylib::Matrix and returns a raylib::Matrix.
template<typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

/* DrawBoundedModel function
*  Parameters: model - raylib::Model&
*              transformer - Transformer auto
*  This function draws the model with the given transformation and then draws a black bounding box around the model.
*/
void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    // Here, we back up the original model transformation matrix in case we want to draw the model again with a different transformation later.
    // The backup will be reapplied to the model after the model is drawn with the current transformation.
    raylib::Matrix backup = model.transform;
    // Here, we apply the transformation from the lambda to our model.
    model.transform = transformer(backup);
    // Here, we draw the model. Notice the vector3 inside the Draw function is empty. This is because we already have our transform applied via the lambda in the line above.
    model.Draw({});
    // Here we create a bounding box object for our model. 
    auto box = model.GetTransformedBoundingBox();
    // Here we draw the bounding box around our model.
    DrawBoundingBox(box, raylib::Color::Black());
    // Here, we reapply the original transformation back to the model.
    // Why is this step necessary? If we want to redraw our models, we need to set the transformation back to the original state so that
    // our transformations are always measured from the origin and not from wherever the previous transformation was. 
    model.transform = backup;
}

int main() {
    // Create environment & camera
    raylib::Window window(1000, 700, "CS381 - Assignment 2");
    cs381::SkyBox sky("textures/skybox.png");
    auto camera = raylib::Camera(
        {0, 120, 500},
        {0, 0, 0},
        {0, 1, 0},
        45
    );

    // Load rocket and truck models
    auto rocket = raylib::Model("meshes/rocket.glb");
    rocket.transform = raylib::Matrix::Identity().Scale(30);
    auto truck = raylib::Model("../assets/Kenny Car Kit/truck.glb");
    truck.transform = raylib::Matrix::Identity().Scale(30);

    // Transformer lambdas for the different model positions
    auto transformerCenter = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({0, 0, 0});
    };
    auto transformerLeft = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({-100, 100, 0}).Scale(1, -1, 1).RotateY(180 * DEG2RAD);
    };
    auto transformerFurtherLeft = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({-200, 0, 0});
    };
    auto transformerFurtherRight = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({200, 0, 0}).RotateY(90 * DEG2RAD);
    };
    auto transformerRight = [](raylib::Matrix& transform) -> raylib::Matrix{
        return transform.Translate({100, 100, 0}).Scale(1, 2, 1).RotateY(270 * DEG2RAD);
    };

    // Draw skybox and models to the window
    while(!window.ShouldClose()) {
        window.BeginDrawing();
            camera.BeginMode();
                window.ClearBackground(raylib::Color::Black());
                sky.Draw();
                DrawBoundedModel(rocket, transformerCenter);
                DrawBoundedModel(rocket, transformerLeft);
                DrawBoundedModel(truck, transformerFurtherLeft);
                DrawBoundedModel(truck, transformerFurtherRight);
                DrawBoundedModel(truck, transformerRight);
            camera.EndMode();
        window.EndDrawing();
    }

    return 0;
}