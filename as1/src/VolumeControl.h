/*******************************************************************************************
*
*   VolumeControl v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_VOLUMECONTROL_IMPLEMENTATION
*       #include "gui_VolumeControl.h"
*
*       INIT: GuiVolumeControlState state = InitGuiVolumeControl();
*       DRAW: GuiVolumeControl(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 Joshua Dahl. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()
#include <cmath>        // Required for fmin(), fmax()

#ifndef GUI_VOLUMECONTROL_H
#define GUI_VOLUMECONTROL_H

typedef struct {
    // Define anchors
    Vector2 anchor01;            // ANCHOR ID:1
    
    // Define controls variables
    float SFXSliderValue;            // Slider: SFXSlider
    float MusicSliderValue;            // Slider: MusicSlider
    float DialogueSliderValue;            // Slider: DialogueSlider

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required
    Sound ping;
    Color backgroundColor;  // Added a ping, background color, and slider select - carlos hernandez
    int sliderSelection;

} GuiVolumeControlState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiVolumeControlState InitGuiVolumeControl(void);
void GuiVolumeControl(GuiVolumeControlState *state, Sound ping); // add ping parameter to gui function - carlos hernandez
// static void PingButton();                // Button: PingButton logic

#ifdef __cplusplus
}
#endif

#endif // GUI_VOLUMECONTROL_H

/***********************************************************************************
*
*   GUI_VOLUMECONTROL IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_VOLUMECONTROL_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiVolumeControlState InitGuiVolumeControl(void) {
    GuiVolumeControlState state = { 0 };

    // Init anchors
    state.anchor01 = (Vector2){ 24, 24 };            // ANCHOR ID:1
    
    // Initilize controls variables
    state.SFXSliderValue = 0.0f;            // Slider: SFXSlider
    state.MusicSliderValue = 0.0f;            // Slider: MusicSlider
    state.DialogueSliderValue = 0.0f;            // Slider: DialogueSlider

    // Custom variables initialization

    state.ping = LoadSound("../audio/ping.wav"); // initialize new variables - carlos hernandez
    state.backgroundColor = WHITE;
    state.sliderSelection = 0;
    return state;
}

// Button: PingButton logic
// static void PingButton()
// {
//     // TODO: Implement in outer file!
// }


void GuiVolumeControl(GuiVolumeControlState *state, Sound ping) {
    // Const text
    const char *VolumeGroupText = "Volume Controls";    // GROUPBOX: VolumeGroup
    const char *SFXGroupText = "SFXVolume";    // GROUPBOX: SFXGroup
    const char *SFXSliderText = "";    // SLIDER: SFXSlider
    const char *MusicGroupText = "MusicVolume";    // GROUPBOX: MusicGroup
    const char *MusicSliderText = "";    // SLIDER: MusicSlider
    const char *DialogueGroupText = "DialogueVolume";    // GROUPBOX: DialogueGroup
    const char *DialogueSliderText = "";    // SLIDER: DialogueSlider
    const char *PingButtonText = "Ping";    // BUTTON: PingButton
    const char *DarkThemeButtonText = "Dark Theme";   // BUTTON: DarkThemeButton
    
    // Background and theme controls for changing between light - dark. - carlos hernandez
    ClearBackground(state->backgroundColor);
    if (state->backgroundColor.r == 0) {
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(LIME));
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(LIME));
        GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, ColorToInt(LIME));
        GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, ColorToInt(LIME));
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(LIME));
        GuiSetStyle(LABEL, BASE_COLOR_NORMAL, ColorToInt(LIME));
    } 
    else {
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
        GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
        GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(DARKGRAY));
        GuiSetStyle(LABEL, BASE_COLOR_NORMAL, ColorToInt(DARKGRAY));
    }

    // Text which changes based on the slider selected
    char *focusText = "";
    if (state->sliderSelection == 0) focusText = "SFX Slider Focused";
    else if (state->sliderSelection == 1) focusText = "Music Slider Focused";
    else if (state->sliderSelection == 2) focusText = "Dialogue Slider Focused";
    GuiLabel((Rectangle){ 24, 370, 256, 24 }, focusText);

    // Draw GUI
    GuiGroupBox((Rectangle){ state->anchor01.x + 0, state->anchor01.y + 0, 256, 264 }, VolumeGroupText);
    GuiGroupBox((Rectangle){ state->anchor01.x + 24, state->anchor01.y + 24, 208, 56 }, SFXGroupText);
    GuiLabel((Rectangle){ 64, 64, 120, 24 }, TextFormat("%.0f%%", state->SFXSliderValue));
    state->SFXSliderValue = GuiSlider((Rectangle){ state->anchor01.x + 72, state->anchor01.y + 40, 144, 24 }, SFXSliderText, NULL, state->SFXSliderValue, 0, 100);
    GuiGroupBox((Rectangle){ state->anchor01.x + 24, state->anchor01.y + 104, 208, 56 }, MusicGroupText);
    GuiLabel((Rectangle){ 64, 144, 120, 24 }, TextFormat("%.0f%%", state->MusicSliderValue));
    state->MusicSliderValue = GuiSlider((Rectangle){ state->anchor01.x + 72, state->anchor01.y + 120, 144, 24 }, MusicSliderText, NULL, state->MusicSliderValue, 0, 100);
    GuiGroupBox((Rectangle){ state->anchor01.x + 24, state->anchor01.y + 184, 208, 56 }, DialogueGroupText);
    GuiLabel((Rectangle){ 64, 224, 120, 24 }, TextFormat("%.0f%%", state->DialogueSliderValue));
    state->DialogueSliderValue = GuiSlider((Rectangle){ state->anchor01.x + 72, state->anchor01.y + 200, 144, 24 }, DialogueSliderText, NULL, state->DialogueSliderValue, 0, 100);
    if (GuiButton((Rectangle){ 24, 304, 256, 24 }, PingButtonText)) PingButton(ping); 
    if (GuiButton((Rectangle){ 24, 336, 256, 24 }, DarkThemeButtonText)) {
        state->backgroundColor = (state->backgroundColor.r == 255) ? BLACK : WHITE;
    }

    // Controls for switching between sliders and changing their values - carlos hernandez
    if (IsKeyPressed(KEY_TAB)) {
        state->sliderSelection = (state->sliderSelection + 1) % 3;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        if (state->sliderSelection == 0) state->SFXSliderValue = fmin(state->SFXSliderValue + 0.01f, 100);
        else if (state->sliderSelection == 1) state->MusicSliderValue = fmin(state->MusicSliderValue + 0.01f, 100);
        else if (state->sliderSelection == 2) state->DialogueSliderValue = fmin(state->DialogueSliderValue + 0.01f, 100);
    }
    if (IsKeyDown(KEY_LEFT)) {
        if (state->sliderSelection == 0) state->SFXSliderValue = fmax(state->SFXSliderValue - 0.01f, 0);
        else if (state->sliderSelection == 1) state->MusicSliderValue = fmax(state->MusicSliderValue - 0.01f, 0);
        else if (state->sliderSelection == 2) state->DialogueSliderValue = fmax(state->DialogueSliderValue - 0.01f, 0);
    }
    // Above, we use fmin and fmax to prevent the sliders from going out of bounds - carlos hernandez
}

#endif // GUI_VOLUMECONTROL_IMPLEMENTATION
