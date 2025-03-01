#include "raylib.h"

#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifndef HOTDOG_COUNTER_H
#define HOTDOG_COUNTER_H

typedef struct {
    Vector2 anchor01;

    int hotdogCounter;
    int highScore;
    float sfxSliderValue;
    float musicSliderValue;
    bool showHighScore;
    bool showAudioControls;
} HotdogCounterState;

HotdogCounterState InitHotdogCounter(void);
void GuiHotdogCounter(HotdogCounterState *state);

#endif 

#if defined(HOTDOG_COUNTER_IMPLEMENTATION)

#include "raygui.h"

HotdogCounterState InitHotdogCounter(void) {
    HotdogCounterState state = { 0 };

    state.anchor01 = (Vector2){ 24, 24 };
    state.hotdogCounter = 0;
    state.highScore = 0;
    state.sfxSliderValue = 50.0f;
    state.musicSliderValue = 50.0f;
    state.showHighScore = false;
    state.showAudioControls = false;

    return state;
}

void GuiHotdogCounter(HotdogCounterState *state) {
    const char *hotdogCounterText = TextFormat("Glizzy Counter: %d", state->hotdogCounter);
    const char *highScoreText = TextFormat("%d", state->highScore);

    Vector2 screenSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    Vector2 highScoreBoxSize = {150, 40};
    Vector2 boxPosition = {screenSize.x - highScoreBoxSize.x - 24, 24};

    Vector2 audioBoxSize = {550, 100};
    float sliderWidth = 300.0f;
    float sliderHeight = 40.0f;
    float centerX = (screenSize.x / 2) - (sliderWidth / 2);
    float centerY = screenSize.y / 2;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiLabel((Rectangle){ state->anchor01.x, state->anchor01.y, 256, 24 }, hotdogCounterText);

    if (state->showHighScore) {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, ColorToInt(BLACK));
        GuiGroupBox((Rectangle){ boxPosition.x - 10, boxPosition.y - 10, highScoreBoxSize.x + 20, highScoreBoxSize.y + 20 }, "High Score");
        GuiLabel((Rectangle){ boxPosition.x, boxPosition.y, highScoreBoxSize.x, highScoreBoxSize.y }, highScoreText);
    }

    if (state->showAudioControls) {
        GuiSetStyle(SLIDER, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        GuiGroupBox((Rectangle){ centerX - 150, centerY - 220, audioBoxSize.x + 20, audioBoxSize.y + 20 }, "Audio Controls");
        state->sfxSliderValue = GuiSlider((Rectangle){centerX + 40, centerY - 200, sliderWidth, sliderHeight}, "SFX Volume", "", state->sfxSliderValue, 0, 100);
        state->musicSliderValue = GuiSlider((Rectangle){centerX + 40, centerY - 150, sliderWidth, sliderHeight}, "Music Volume", "", state->musicSliderValue, 0, 100);
    }
}

#endif