# Assignment 0 - Carlos Hernandez - CS 381

## How to Make a Resizeable Window using Raylib
Making a resizeable window using C++'s `raylib` library is easy and only takes a couple steps. <br/>
Using raylibs `Window` class, we want to first create a Window object whose constructor accepts <br />
three parameters: width, height, (both in pixels) and a flag argument. The flag argument we want <br />
to pass in is the `FLAG_WINDOW_RESIZEABLE` enum which should now make our window resizeable. <br />

## How to Keep Text Centered in a Resizeable Window
Keeping text centered within a resizable window is simple and only requires a few steps. <br/>
First, we want to gather some information about our window and text such as the window height and <br />
width using the `GetWidth()` and `GetHeight()` methods from the `Window` class and the text width <br />
and height using the `Measure()` method for text width and `GetFontSize()` method for text height, <br />
both coming from the `Text` class. To now calculate the width and height of our text with respect <br />
to the window size, we use the formula `(windowWidth - textWidth) / 2` (same formula for height) which we <br />
can save into a variable which stores the true position of the text, and in the `Draw()` method for <br />
displaying text to the window we can insert our variables like so: `sampleString.Draw({trueX, trueY})` <br/>

## How to Fetch the Raylib Submodule
The following steps will walk you though how to fetch the `raylib-cpp` submodule. <br />
- If you haven't already, create a folder for your project: `mkdir myProject` and `cd myProject`
- Next, you want to initialize a git repository: `git init`
- You are now ready to add the raylib submodule! Do the following command in the terminal: `git submodule add https://github.com/joshuadahlunr/raylib-cpp.git`
- Our submodule is not quite ready, we need to initialize its dependecies. Run: `git submodule init` and `git submodule update --init --recursive`
- `raylib-cpp` is now successfully downloaded to your folder!.

## How to Compile AS0
Compiling the code within the as0 folder is easy and only takes a few steps. <br />
- Open the parent folder `CS 381` in a terminal and type `cd as0`
- Next, we will cd one more time into the build folder: `cd build`
- In this folder, we can run the command `cmake ..` which will generate a makefile for us.
- We can now type `make` in the terminal to compile our code.

## How to Run AS0
If you successfully followed the steps in the How to Compile section above, you should have successfully <br />
compiled the code for as0. You may have noticed that in the `build` folder we have a new file simply called <br />
`as0` This is the executable for the program, and can simply be run via the following command: <br />
`as0` <br />
Congratulations! You've successfully ran as0. <br />

## How to enumerate the controls for AS0
- not applicable