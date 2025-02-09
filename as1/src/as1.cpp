#include <raylib-cpp.hpp>
#include "VolumeControl.h"

void PingButton(Sound ping) {
    PlaySound(ping);
}

int main() {
    raylib::Window window(300, 400, "CS381 - Assignment 1");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    GuiVolumeControlState volumeControlGui = InitGuiVolumeControl();

    raylib::AudioDevice audioDevice; 
    audioDevice.Init();
    Sound ping = LoadSound("../audio/ping.wav");
    raylib::Music backgroundNoise = LoadMusicStream("../audio/crowd.wav");
    raylib::Music music = LoadMusicStream("../audio/price-of-freedom.mp3");

    PlayMusicStream(backgroundNoise);
    PlayMusicStream(music);

    while(!window.ShouldClose()) {
        SetSoundVolume(ping, volumeControlGui.SFXSliderValue / 20.0f);
        SetMusicVolume(backgroundNoise, volumeControlGui.DialogueSliderValue / 50.0f);
        UpdateMusicStream(backgroundNoise);
        SetMusicVolume(music, volumeControlGui.MusicSliderValue / 20.0f);
        UpdateMusicStream(music);

        window.BeginDrawing();
            window.ClearBackground(raylib::Color::White());
            GuiVolumeControl(&volumeControlGui, ping);
        window.EndDrawing();
    }

    UnloadMusicStream(backgroundNoise);
    UnloadMusicStream(music);
    UnloadSound(ping);
    
    audioDevice.Close();

    return 0;
}