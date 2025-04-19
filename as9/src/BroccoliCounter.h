#include "raylib.h"

#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifndef BROCCOLI_COUNTER_H
#define BROCCOLI_COUNTER_H

typedef struct {
    Vector2 anchor01;

    int broccoliCounter;
    bool startGame;
} BroccoliCounterState;

BroccoliCounterState InitBroccoliCounter(void);
void GuiBroccoliCounter(BroccoliCounterState *state);

#endif

#if defined(BROCCOLI_COUNTER_IMPLEMENTATION)

#include "raygui.h"

BroccoliCounterState InitBroccoliCounter(void) {
    BroccoliCounterState state = { 0 };

    state.anchor01 = (Vector2){ 24, 24 };
    state.broccoliCounter = 0;
    state.startGame = false;

    return state;
}

void GuiBroccoliCounter(BroccoliCounterState *state) {
    const char *broccoliCounterText = TextFormat("Broccoli Counter: %d", state->broccoliCounter);
    const char *startGameText = "Start Game";

    Vector2 screenSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    
    Vector2 counterBoxSize = {256, 24};
    Vector2 counterPosition = {screenSize.x - counterBoxSize.x - 24, 24};
    
    Vector2 startGameBoxSize = {150, 40};
    Vector2 centerPosition = {
        screenSize.x / 2 - startGameBoxSize.x / 2,
        screenSize.y / 2 - startGameBoxSize.y / 2
    };

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiLabel((Rectangle){ counterPosition.x, counterPosition.y, counterBoxSize.x, counterBoxSize.y }, broccoliCounterText);

    if (!state->startGame) {
        if (GuiButton((Rectangle){ centerPosition.x, centerPosition.y, startGameBoxSize.x, startGameBoxSize.y }, startGameText)) {
            state->startGame = true;
        }
    }
}

#endif