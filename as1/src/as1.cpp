#include <raylib-cpp.hpp>
#include "VolumeControl.h"

// Implement PingButton, added parameter for ping sound
void PingButton(Sound ping) {
    PlaySound(ping);
}

int main() {
    // Create window
    raylib::Window window(300, 400, "CS381 - Assignment 1");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    // Create gui
    GuiVolumeControlState volumeControlGui = InitGuiVolumeControl();

    // Declare audio variables
    raylib::AudioDevice audioDevice; 
    audioDevice.Init();
    Sound ping = LoadSound("../audio/ping.wav");
    raylib::Music backgroundNoise = LoadMusicStream("../audio/crowd.wav");
    raylib::Music music = LoadMusicStream("../audio/price-of-freedom.mp3");

    // Begin playing audio
    PlayMusicStream(backgroundNoise);
    PlayMusicStream(music);

    while(!window.ShouldClose()) {
        // Update audio based on the slider values
        SetSoundVolume(ping, volumeControlGui.SFXSliderValue / 20.0f);
        SetMusicVolume(backgroundNoise, volumeControlGui.DialogueSliderValue / 50.0f);
        UpdateMusicStream(backgroundNoise);
        SetMusicVolume(music, volumeControlGui.MusicSliderValue / 20.0f);
        UpdateMusicStream(music);

        // Draw the gui
        window.BeginDrawing();
            window.ClearBackground(raylib::Color::White());
            GuiVolumeControl(&volumeControlGui, ping);
        window.EndDrawing();
    }

    // Unload audio and close audio device
    UnloadMusicStream(backgroundNoise);
    UnloadMusicStream(music);
    UnloadSound(ping);
    
    audioDevice.Close();

    return 0;
}