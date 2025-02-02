#include "Color.hpp"
#include <iostream>

#include <raylib-cpp.hpp>

int main() {
    // Create Resizeable Window
    raylib::Window window(800, 400, "CS381 - Assignment 0");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    // Create text strings, three for three different colors
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

    // Variables for keeping track of text animation position & speed.
    float heightOffset = 0.0;
    float verticalSpeed = 0.01;
    bool movingDown = true;

    // Main Loop
    while(!window.ShouldClose()) {
        // Get text positions with respect to window size
        int width = window.GetWidth();
        int height = window.GetHeight();
        int textOneWidth = textStringOne.Measure();
        int textTwoWidth = textStringTwo.Measure();
        int textThreeWidth = textStringThree.Measure();
        int textHeight = textStringOne.GetFontSize();

        float totalTextWidth = textOneWidth + textTwoWidth + textThreeWidth;
        float textXPosition = (width - totalTextWidth) / 2;
        float textYPosition = (height - textHeight) / 2;

        // This is for modifying the space between the text strings
        float textGap = 20.0;

        /* How the text animation works. We want the animation to have an up-down range of 100 (-50 to 50)
        *  pixels. So once the height offset reaches 50 pixels in either direction we change directions.
        */
        if (movingDown) {
            heightOffset = heightOffset + verticalSpeed;
            if (heightOffset >= 50) {
                movingDown = false;
            }
        } 
        else {
            heightOffset = heightOffset - verticalSpeed;
            if (heightOffset <= -50) {
                movingDown = true;
            }
        }

        // Draw the text strings
        window.BeginDrawing();
            window.ClearBackground(raylib::Color::White());
            textStringOne.Draw({textXPosition, textYPosition + heightOffset});
            textStringTwo.Draw({textXPosition + textOneWidth, textYPosition});
            textStringThree.Draw({textXPosition + textOneWidth + textTwoWidth - textGap, textYPosition});
        window.EndDrawing();
    }

    return 0;
}