#include "raylib.h"

#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifndef FLAPPY_GUI_H
#define FLAPPY_GUI_H

typedef struct {
    float score;
    bool showStartScreen;
    bool showGameOverScreen;
} FlappyCounterState;

FlappyCounterState InitFlappyCounter(void);
void GuiFlappyCounter(FlappyCounterState *state);

#endif

#if defined(FLAPPY_GUI_IMPLEMENTATION)

#include "raygui.h"
FlappyCounterState InitFlappyCounter(void) {
    FlappyCounterState state = { 0 };
    state.score = 0.0f;

    return state;
}

void GuiFlappyCounter(FlappyCounterState *state) {
    int score = (int)state->score;
    const char *scoreText = TextFormat("Score: %d", score);
    Vector2 screenSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    Vector2 boxSize = {200, 40};
    Vector2 boxPosition = {screenSize.x - boxSize.x - 24, 24};

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiLabel((Rectangle){ boxPosition.x, boxPosition.y, boxSize.x, boxSize.y }, scoreText);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiLabel((Rectangle){ boxPosition.x, boxPosition.y, boxSize.x, boxSize.y }, scoreText);

    if (state->showStartScreen) {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        float textWidth = MeasureText("Press SPACE to Start", 48);
        GuiLabel((Rectangle){ screenSize.x / 2 - textWidth / 2, screenSize.y / 2 - 50, textWidth, 100 }, "Press SPACE to Start");
    }

    if (state->showGameOverScreen) {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        float textWidth = MeasureText("Game Over! Press R to Restart", 48);
        GuiLabel((Rectangle){ screenSize.x / 2 - textWidth / 2, screenSize.y / 2 - 50, textWidth, 100 }, "Game Over! Press R to Restart");
    }
}

#endif