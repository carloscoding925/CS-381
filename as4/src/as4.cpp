#include "Color.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include <raylib-cpp.hpp>
#include "../assets/skybox.hpp"
#include <iostream>
#include <raymath.h>
#include "HotdogCounter.h"
#include <fstream>

// Enforce raylib::Matrix 
template<typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

// Draw model function
void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    auto box = model.GetTransformedBoundingBox();
    DrawBoundingBox(box, raylib::Color::Black());
    model.transform = backup;
}

// Check if toilet is within a certain radius of the hot dog
bool checkCollision(raylib::Vector3& toilet, raylib::Vector3& hotdog) {
    float radius = 5.0f;
    if (std::fabs(toilet.x - hotdog.x) < radius && std::fabs(toilet.z - hotdog.z) < radius) {
        return true;
    }

    return false;
}

// Generate new location for the hotdog within a -100 to 100 bounded area
raylib::Vector3 generateNewLocation() {
    float x = std::rand() % 200 - 100;
    float z = std::rand() % 200 - 100;
    return raylib::Vector3{x, 0.0f, z};
}

// Costco guys ascii art
std::string costcoGuysASCIIArt() {
    return R"(
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣰⣶⣿⣿⣿⣿⣿⣷⣦⣄⡀⠀⠀⠀⠀⠀⠀⠀⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡴⣖⣻⢿⣹⢯⣟⣟⣻⢿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⢠⣞⢧⣻⣭⣟⣯⣿⡿⡾⢿⣭⣿⣞⡿⢿⣿⣿⡀⠀⠀⠀⠀⣿⣿⣿⣿⠿⠿⠟⠻⠛⢿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⡟⣼⢇⣿⣼⣿⡿⣇⠻⢤⢣⢼⣻⢿⣿⣿⣻⣿⣧⠀⠀⠀⠀⣿⣿⣿⢃⠀⠀⠀⠀⠀⠀⠘⢿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠻⡝⢮⣿⣿⢏⠓⠌⠉⢂⠈⠀⢁⣉⣘⡹⠿⣿⣿⠀⠀⠀⠀⣿⣿⣻⠄⠂⠀⠀⠀⣀⣠⣤⡀⠘⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⢿⣷⢾⣶⣶⡧⠠⢀⣿⣿⣛⣛⡿⢦⠀⢿⣀⠀⠀⢀⣽⣿⣿⣿⣿⣇⠀⢻⣿⣷⠶⠍⠂⠝⢻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣳⢟⣻⣿⠫⣕⠢⢞⣿⢟⣚⡡⡔⡌⣻⢼⠀⠀⠈⣿⡎⠉⠉⢾⣻⠂⠄⠁⠂⠁⠀⠀⡸⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢨⣱⢏⣼⡷⠓⡨⠇⠾⢧⣏⢲⠻⣢⠱⣝⢏⠀⠀⠀⣿⡧⡁⢠⢿⣧⡄⠤⠹⣦⠀⠀⠀⣅⣵⡤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠧⣿⡿⣳⠟⢖⣻⠿⢌⢻⣷⡆⢱⠝⣸⡸⠀⠀⣀⣬⣷⡇⢿⣶⣶⣼⣶⣦⠾⠁⠀⢸⠀⣿⢳⣍⢻⡭⣓⡷⢦⣄⡀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⡋⡷⣷⣟⡶⠖⣒⣖⡾⡟⠋⠄⢱⡟⣠⣶⣿⣿⣿⣿⣷⣈⢎⠉⠉⠉⠀⠀⠀⠄⢁⣶⣿⠳⣎⠷⣝⢣⡜⢦⡘⣍⢳⢄⡀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠩⡧⠡⡙⢻⣓⣒⡛⠉⠁⠁⢀⡐⣼⠃⣿⣿⣿⣿⣿⣿⣿⣯⡘⡀⠀⠀⠀⣀⠔⠀⣼⣿⣏⡲⣙⠶⣉⢮⡑⣎⡱⣉⠞⡜⢁⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⢇⠈⡜⢡⠡⡌⠀⠀⠀⠄⡼⠊⡔⢡⡏⣿⣿⣿⣿⣿⣿⣿⣶⣦⣖⣋⣀⡴⣾⣿⠏⡴⢣⣽⢡⢩⢲⣽⡃⢖⡡⠎⡔⢂⠢⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣾⣷⣅⣇⡰⣠⣌⡢⢖⠏⡁⠐⣈⠢⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣮⣡⣿⣿⣫⣜⣽⣿⠣⢎⡖⡿⢎⡱⢢⡟⡰⣑⠪⢄⠣⠀
    ⠀⠀⠀⠀⠀⠀⣠⣶⣶⣤⣾⣿⣿⣿⣿⣿⡏⢭⣻⠓⠮⣀⠩⡐⠡⡐⢡⣿⣿⣿⣿⣿⣻⣯⣿⣿⡟⠏⣭⠋⠽⢉⠙⠛⢿⣪⣝⣫⣼⡱⢎⡴⣻⠴⡱⢌⠳⡌⡔⢂
    ⠀⠀⠀⡔⣲⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡷⠅⠆⠐⠂⠁⣡⣼⣿⣿⣿⣿⣯⣷⣿⣻⢷⣯⣟⡙⠐⠒⠃⠄⢀⠀⠀⠉⠻⣏⣷⢻⣜⣾⣧⢗⡱⢪⡱⠜⡤⡉
    ⠀⠀⣮⣵⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣿⣿⣿⣿⣆⢀⡀⣠⣾⣿⣿⡿⣿⣟⡿⣿⡾⣯⣟⣟⡾⣿⣿⡟⠓⠒⠤⠀⡁⠀⠀⠀⠈⠺⢿⣾⣿⣭⣎⣵⣃⣞⢱⢒⡡
    ⠀⢠⣾⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⢻⡟⣿⣿⣿⣿⣿⣿⣿⢻⣿⣿⣿⣿⡟⣾⡟⣿⣽⡗⣿⡞⣿⣿⣽⣿⣷⣶⣬⠀⠀⠀⠀⠀⠀⠀⠀⠉⢻⣿⣿⣶⣿⣬⢫⡎⡔
    ⠀⣸⣿⣿⣿⣽⣾⣿⣿⣿⡿⣿⣽⣿⣿⢿⣷⡿⣿⣿⣿⢿⣟⣯⣷⣻⢷⣻⣽⢿⣽⣳⢿⣽⣳⡟⣿⣽⣿⣿⡿⣿⣿⣶⣤⣀⡀⠀⠀⠀⠀⠀⠀⠉⠑⠾⢵⣋⡩⢔
    ⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣟⣿⣾⢿⣯⣿⢿⣿⢾⣿⢿⣯⣷⣿⢿⣯⢿⣞⡷⣟⣾⣻⢾⣽⠿⣿⣯⣷⣯⣿⣿⢿⣽⣿⣿⢿⣷⣄⡀⢀⠀⠀⠀⠀⠀⠀⠀⠑⠌
    ⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣻⣽⡿⣟⣿⣿⣻⣿⣯⣿⣻⢿⣽⣻⣾⢯⡿⣽⣻⣽⢯⣟⣿⣟⣿⣿⣿⣿⣿⣿⢿⣽⢷⣞⣻⣿⣿⣿⣶⣎⡖⣄⡂⡄⢠⢀⣀⠀)";
}

// chillguy ascii art
std::string chillGuyASCIIArt() {
    return R"(
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⣷⣄⠀⠀⠀⣀⣤⣤⣤⡀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣶⠏⠀⠀⣿⠀⢀⡾⠛⠋⠀⣾⣿⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⡏⠀⠀⠀⣿⢀⣾⠁⠀⣰⠆⢹⡿⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠃⣧⠀⠀⢠⡟⢸⡇⠀⣰⠟⠀⣼⠃⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣹⣆⢀⣸⣇⣸⠃⢠⡏⠀⣸⠋⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣴⣶⣶⣶⠾⠟⠛⠉⠉⠉⠈⠉⠉⠛⠁⢾⠁⣴⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣤⣤⣶⣶⠾⠟⠛⠛⣻⣿⣙⡁⠀⠀⢾⣶⣾⣷⣿⣶⣄⠀⠀⠀⠀⠰⢿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⢀⣀⣀⣀⣠⣴⣶⣶⠾⠟⠛⠉⠉⠉⠀⠀⠀⠀⠀⣿⣻⣟⣻⣿⡦⠀⠘⣿⣿⣛⡿⢶⡇⠀⠀⠀⠀⠀⠀⢻⣆⠀⠀⠀⠀⠀⠀⠀⠀
    ⣠⣶⣶⣶⣾⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡟⠙⣿⣿⡗⠀⠀⠿⠉⣿⣿⣿⣶⠀⠀⠀⠀⠀⠀⠈⢿⠀⠀⠀⠀⠀⠀⠀⠀
    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠳⣄⣿⡿⠁⠀⠀⠘⢦⣿⣿⠇⠟⠁⠀⠀⠀⠀⠀⠀⣸⡇⠀⠀⠀⠀⠀⠀⠀
    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⡇⠀⠀⠀⠀⠀⠀⠀
    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣇⠀⠀⠀⠀⠀⠀⠀
    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⠀⠀⠀⠀⠀⠀⠀
    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡏⠀⠀⠀⠀⠀⠀⠀
    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀
    ⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⡇⠀⠀⠀⠀⠀⠀⠀
    ⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢤⣤⡀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠈⠙⢿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣤⡾⠟⠛⠆⠀⠀⠀⠀⠀⢀⢻⡇⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠈⠙⠿⣿⣭⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣴⣶⠾⠟⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣾⠇⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠙⠛⠷⠶⢶⣶⣦⣤⣴⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣌⣿⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠙⠛⠛⠛⠃⠀⠀⠀⠀⠀⠀⠀⣤⣴⣾⣿⣿⣿⣓⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⣿⣷⣦⣄⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣠⣤⣶⣾⣟⣯⣽⠟⠋⠀⠉⠳⣄⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⢇⠀⠉⠛⠷⣮⣍⣩⡍⢻⡟⠉⣉⢹⡏⠉⣿⣹⣷⣦⣿⠿⠟⠉⠀⠀⠀⠀⠀⠀⠙⣆⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠏⢸⠇⠀⠀⠀⠀⠀⠉⠉⠛⠛⠛⠛⠛⠛⠛⠋⠉⠉⠀⠀⠀⠀⠀⢠⣠⡶⠀⠀⠀⠀⠘⣧⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⡿⠀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠟⠁⠀⠀⠀⠀⠀⠘⣆⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⠃⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣇⡀⠀⠀⠀⠀⠀⠀⢹⡆⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣾⠀⣾⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣥⢠⣤⠼⠇⠀⠀⠘⣿⡄
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣽⡄⠈⢿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠿⠾⠷⠄⠀⠀⠀⢀⣿⠁
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣧⠀⠸⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣤⣾⠋⠀⠀⠀⠀⠀⠀⢰⣾⡿⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣦⣠⣿⣿⣶⣶⣤⣤⣄⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣠⣴⣿⣇⠀⠀⠀⠀⠀⠀⠀⣸⡟⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⠀⠉⠛⢿⣿⣯⣿⡟⢿⠻⣿⢻⣿⢿⣿⣿⣿⣿⣿⠿⠟⠹⣟⢷⣄⠀⠀⠀⢀⣼⠟⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣄⠀⠀⠘⢷⣌⡻⠿⣿⣛⣿⣟⣛⣛⣋⣉⣉⣉⣀⡀⠀⠀⠈⠻⢿⣷⣶⣶⢛⣧⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣏⠀⠀⠀⠀⠹⢯⣟⣛⢿⣿⣽⣅⣀⡀⠀⣀⡀⠀⠀⠀⠠⢦⣀⠰⡦⠀⢸⠀⣏⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⡀⠀⠀⠀⠀⠀⠀⠈⠉⢻⣿⡟⠛⠉⠉⠁⠀⠀⠀⠀⠀⠀⠈⠛⠷⠀⣸⠀⣿⡀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣧⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⣿⡇⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⢿⠀⠀⢦⡀⡀⠀⠀⠀⠀⢹⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡄⡏⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡄⠀⠈⠳⣝⠦⢄⠀⠀⠀⣟⣷⠀⠀⠀⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⡇⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣄⣷⡀⠀⠀⠈⠙⠂⠀⠀⠀⢸⣿⡄⠀⠀⠘⢦⡙⢦⡀⠀⠀⠀⠀⢰⣷⣷⡇⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⡿⢧⣤⣀⡀⠀⠀⠀⠀⠀⠀⢿⣷⣄⠀⠀⠀⠁⠋⠀⠀⠀⠀⠀⢸⣿⣿⣇⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣷⡀⠈⠉⠛⠛⠛⠛⠛⠛⠛⠛⢿⡍⠛⠳⠶⣶⣤⣤⣤⣤⣤⣤⠼⠟⡟⢿⡇⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⣾⡇⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣤⣴⣿⣷⣶⣶⣶⣶⣶⣶⣦⣀⣀⣀⣻⡀⠀⠀⠀⣀⣀⠀⡀⠀⠀⠀⢀⣼⣿⠇⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠟⠉⠁⠀⠀⠈⠻⣿⡆⢹⣯⣽⣿⣿⠟⠋⠙⣿⣶⣿⣿⣿⣿⣾⣿⣿⣿⣟⠋⠉⣇⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇⠀⠀⠀⡀⠀⠀⠀⠈⢻⣆⣿⠀⠀⠀⢁⣶⣿⠿⠟⠛⠷⣶⣽⣿⣿⣻⣏⠙⠃⣴⢻⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣷⣀⠀⠀⠉⠀⠀⠀⠀⠀⢹⣿⠀⣀⣴⣿⠋⠀⠀⠀⠀⠀⠀⠉⠻⣿⣧⣿⢀⣰⣿⣿⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢿⣶⣶⣤⣤⣤⣤⣤⣤⣾⣿⣟⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣅⣾⢿⣵⠇⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⠛⠛⠛⠛⠛⠛⠛⠉⠉⠉⠁⢹⣜⠷⠦⠤⠤⠤⠤⠤⠴⠶⠛⣉⣱⠿⠁⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠿⠷⣦⣤⣤⣄⣠⣤⣤⡶⠟⠁⠀⠀⠀⠀⠀⠀⠀
    )";
}

int main() {
    // Print ascii art
    std::cout << costcoGuysASCIIArt() << std::endl;
    std::cout << chillGuyASCIIArt() << std::endl;

    // Window variables
    const int windowWidth = 1000;
    const int windowHeight = 700;
    const std::string title = "The Rizzler & Big AJ Present: Skibidi Toilet Glizzy Gobbler";
    raylib::Window window(windowWidth, windowHeight, title);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    // Audio variables
    raylib::AudioDevice audioDevice;
    Sound glizzy = LoadSound("../assets/audio/squidward-says-glizzy.mp3");
    raylib::Music costcoGuys = LoadMusicStream("../assets/audio/we-bring-the-boom.mp3");

    costcoGuys.SetLooping(true);
    PlayMusicStream(costcoGuys);

    HotdogCounterState hotdogState = InitHotdogCounter();

    // Models
    raylib::Model toilet("../assets/Kenny-Furniture-Kit/toilet.glb");
    toilet.transform = raylib::Matrix::Identity().Scale(5);
    raylib::Model hotdog("../assets/Kenny-Food-Kit/meat-sausage.glb");
    hotdog.transform = raylib::Matrix::Identity().Scale(15);

    raylib::Model floor = raylib::Mesh::Plane(200, 200, 1, 1).LoadModelFrom();
    raylib::Texture floorTexture = raylib::Texture("../assets/textures/floor.png");
    floor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTexture;

    // Sky and Camera
    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera(raylib::Vector3{0.0f, 20.0f, 40.0f}, raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector3{0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    // Toilet Physics
    raylib::Vector3 toiletPosition = { 0.0f, 0.0f, 0.0f };
    raylib::Vector3 hotdogPosition = { 0.0f, 0.0f, -50.0f };

    float targetSpeed = 0;
    float toiletSpeed = 0;
    float toiletHeading = 0;
    float toiletYTarget = 0;
    bool showHighScore = false;

    // Read high score from file
    std::ifstream readFile("../assets/data/score.txt");
    if (readFile.is_open()) {
        readFile >> hotdogState.highScore;
        readFile.close();
    }
    else {
        std::cout << "Unable to open score file" << std::endl;
    }

    // Main game loop
    while(!window.ShouldClose()) {
        // Update music and sound volume
        SetMusicVolume(costcoGuys, hotdogState.musicSliderValue / 100.0f);
        UpdateMusicStream(costcoGuys);
        SetSoundVolume(glizzy, hotdogState.sfxSliderValue / 100.0f);

        // Handle user input
        if (raylib::Keyboard::IsKeyDown(KEY_W)) {
            targetSpeed = 30.0f;
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_S)) {
            targetSpeed = -30.0f;
        } 
        else {
            targetSpeed = 0.0f;
        }

        if (raylib::Keyboard::IsKeyDown(KEY_A)) {
            toiletHeading = toiletHeading + (100.0f * window.GetFrameTime());
            if (toiletHeading > 360.0f) {
                toiletHeading = toiletHeading - 360.0f;
            }
        }
        else if (raylib::Keyboard::IsKeyDown(KEY_D)) {
            toiletHeading = toiletHeading - (100.0f * window.GetFrameTime());
            if (toiletHeading < -360.0f) {
                toiletHeading = toiletHeading + 360.0f;
            }
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_SPACE)) {
            toiletYTarget = 4.0f;
        }
        else if (toiletPosition.y > 3.5f){
            toiletYTarget = 0;
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_T)) {
            showHighScore = !showHighScore;
            hotdogState.showHighScore = showHighScore;
        }
        if (raylib::Keyboard::IsKeyPressed(KEY_V)) {
            hotdogState.showAudioControls = !hotdogState.showAudioControls;
        }

        // Calculate toilet position and update camera
        float radians = DEG2RAD * toiletHeading;
        toiletSpeed = std::lerp(toiletSpeed, targetSpeed, window.GetFrameTime());
        toiletPosition.y = std::lerp(toiletPosition.y, toiletYTarget, window.GetFrameTime() * 2);
        raylib::Vector3 velocity = { cos(radians) * toiletSpeed, 0, -sin(radians) * toiletSpeed };
        toiletPosition = toiletPosition + velocity * window.GetFrameTime();

        camera.position = toiletPosition + raylib::Vector3{0.0f, 20.0f, 40.0f};
        camera.target = toiletPosition;

        // Check if player has collided with hotdog
        if (checkCollision(toiletPosition, hotdogPosition)) {
            hotdogPosition = generateNewLocation();
            hotdogState.hotdogCounter++;
            PlaySound(glizzy);
        }

        // Update high score if player has beaten it
        if (hotdogState.hotdogCounter > hotdogState.highScore) {
            hotdogState.highScore = hotdogState.hotdogCounter;
        }

        // Bound toilet to playable environment
        if (toiletPosition.x > 100.0f) {
            toiletPosition.x = 100.0f;
        }
        else if (toiletPosition.x < -100.0f) {
            toiletPosition.x = -100.0f;
        }
        else if (toiletPosition.z > 100.0f) {
            toiletPosition.z = 100.0f;
        }
        else if (toiletPosition.z < -100.0f) {
            toiletPosition.z = -100.0f;
        }

        // Draw the environment
        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::White());
            camera.BeginMode();
            {
                sky.Draw();
                floor.Draw({});
                DrawBoundedModel(toilet, [&toiletPosition, &toiletHeading](raylib::Transform t) {
                    return t.Translate(toiletPosition).RotateY(raylib::Degree(toiletHeading));
                });
                DrawBoundedModel(hotdog, [&hotdogPosition](raylib::Transform t) {
                    return t.Translate(hotdogPosition);
                });
            }
            camera.EndMode();
            GuiHotdogCounter(&hotdogState); 
        }
        window.EndDrawing();

    }

    // Unload resources
    UnloadMusicStream(costcoGuys);
    UnloadSound(glizzy);
    audioDevice.Close();

    // Save high score to file
    std::ofstream outFile("../assets/data/score.txt", std::ofstream::out);
    if (outFile.is_open()) {
        if (hotdogState.hotdogCounter > hotdogState.highScore) {
            outFile << hotdogState.hotdogCounter;
            outFile.close();
        }
        else {
            outFile << hotdogState.highScore;
            outFile.close();
        }
    }
    else {
        std::cout << "Unable to open score file" << std::endl;
    }

    return 0;
}