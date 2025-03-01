#include "raylib.h"

#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifndef HOTDOG_COUNTER_H
#define HOTDOG_COUNTER_H

typedef struct {
    Vector2 anchor01;

    int hotdogCounter;
    int highScore;
    bool showHighScore;
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
    state.showHighScore = false;

    return state;
}

void GuiHotdogCounter(HotdogCounterState *state) {
    const char *hotdogCounterText = TextFormat("Glizzy Counter: %d", state->hotdogCounter);
    const char *highScoreText = TextFormat("%d", state->highScore);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiLabel((Rectangle){ state->anchor01.x, state->anchor01.y, 256, 24 }, hotdogCounterText);

    if (state->showHighScore) {
        Vector2 screenSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
        Vector2 boxSize = {200, 50};
        Vector2 boxPosition = {screenSize.x - boxSize.x - 24, 24};

        GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, ColorToInt(BLACK));
        GuiGroupBox((Rectangle){ boxPosition.x - 10, boxPosition.y - 10, boxSize.x + 20, boxSize.y + 20 }, "High Score");
        GuiLabel((Rectangle){ boxPosition.x, boxPosition.y, boxSize.x, boxSize.y }, highScoreText);
    }
}

#endif