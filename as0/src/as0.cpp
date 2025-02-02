#include "Color.hpp"
#include <iostream>

#include <raylib-cpp.hpp>

int main() {
    raylib::Window window(800, 400, "CS381 - Assignment 0");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Text textStringOne;
    raylib::Text textStringTwo;
    raylib::Text textStringThree;

    textStringOne.SetText("Congratulations!");
    textStringOne.SetFontSize(24);
    textStringOne.SetColor(raylib::Color::Red());

    textStringTwo.SetText("Assignment 0 is");
    textStringTwo.SetFontSize(24);
    textStringTwo.SetColor(raylib::Color::Black());

    textStringThree.SetText("complete!");
    textStringThree.SetFontSize(24);
    textStringThree.SetColor(raylib::Color::DarkGreen());

    while(!window.ShouldClose()) {
        int width = window.GetWidth();
        int height = window.GetHeight();
        int textOneWidth = textStringOne.Measure();
        int textTwoWidth = textStringTwo.Measure();
        int textThreeWidth = textStringThree.Measure();
        int textHeight = textStringOne.GetFontSize();

        float totalWidth = textOneWidth + textTwoWidth + textThreeWidth;
        float textXPosition = (width - totalWidth) / 2;
        float textYPosition = (height - textHeight) / 2;

        float textGap = 20.0f;

        window.BeginDrawing();
            window.ClearBackground(raylib::Color::White());
            textStringOne.Draw({textXPosition, textYPosition});
            textStringTwo.Draw({textXPosition + textOneWidth, textYPosition});
            textStringThree.Draw({textXPosition + textOneWidth + textTwoWidth - textGap, textYPosition});
        window.EndDrawing();
    }

    return 0;
}