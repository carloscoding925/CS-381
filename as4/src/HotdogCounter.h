#include "raylib.h"

#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifndef HOTDOG_COUNTER_H
#define HOTDOG_COUNTER_H

typedef struct {
    Vector2 anchor01;
    int hotdogCounter;
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

    return state;
}

void GuiHotdogCounter(HotdogCounterState *state) {
    const char *hotdogCounterText = TextFormat("Glizzy Counter: %d", state->hotdogCounter);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiLabel((Rectangle){ state->anchor01.x, state->anchor01.y, 256, 24 }, hotdogCounterText);
}

#endif