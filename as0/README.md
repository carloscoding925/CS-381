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
- First, we will create the directory where we will store our code.
- We will make our new folder within the `Documents` directory so run the following: `mkdir ~/Documents/as0`
- Lets navigate to our new folder: `cd ~/Documents/as0`
- Next, you want to initialize a git repository: `git init`
- You are now ready to add the raylib submodule! Do the following command in the terminal: `git submodule add https://github.com/joshuadahlunr/raylib-cpp.git`
- Our submodule is not quite ready, we need to initialize its dependecies. Run: `git submodule init` and `git submodule update --init --recursive`
- `raylib-cpp` is now successfully downloaded to your folder!

## How to Compile AS0
Compiling the code within the as0 folder is easy and only takes a few steps. <br />
- For this section we'll build on the directory we created in the previous section. 
- Lets open our folder from the previous section: `cd ~/Documents/as0`
- Here, created a `src` folder to store our source code: `mkdir src`
- From this github repo open the `as0` folder, then the `src` folder and download `as0.cpp` to your Downloads folder.
- Lets move the file from Downloads into our project folder: `mv ~/Downloads/as0.cpp ~/Documents/as0/src/`
- Go ahead and download `CMakeLists.txt` from the `as0` folder and again we'll move it into our project folder:
- `mv ~/Downloads/CMakeLists.txt ~/Documents/as0/`
- At this point we should still be in `~/Documents/as0` but if not navigate back here via `cd ~/Documents/as0`
- We want to create a `build` folder to store our Makefile and other necessities so run the following: `mkdir build`
- Next, we will cd one more time into the build folder: `cd build`
- In this folder, we can run the command `cmake ..` which will generate a makefile for us.
- We can now type `make` in the terminal to compile our code.
<br />
Congrats! The code should now be successfully compiled. <br/>

## How to Run AS0
If you successfully followed the steps in the How to Compile section above, you should have successfully <br />
compiled the code for as0. You may have noticed that in the `build` folder we have a new file simply called <br />
`as0`. This is the executable for the program, and can simply be run via the following steps: <br />
Note: the following `cd` assumes you've been following the steps in the fetch raylib and compile as0. <br />
- cd into the build folder: `cd ~/Documents/as0/build`
- run the following: `./as0` 
<br/>
Congratulations! You've successfully ran as0. <br />

## How to enumerate the controls for AS0
- not applicable