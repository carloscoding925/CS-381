# Assignment 3 - Carlos Hernandez - CS 381

## What does DT stand for, why is it important, and how do we calculate it?
DT stands for `delta time` and it typically represents a change in time. In the context of this assignment, <br />
DT represents the time between frames (specifically the current and previous frame). In our program, DT is <br />
calculated using the `GetFrameTime()` function which returns the time that has passed since the last frame was shown <br />
with the equation for this simply being `currentTime - previousTime`. Calculating dt and using it in our position <br />
equation makes it so that our models movement is clean and consistent regardless of the performance constraints of <br />
whatever device the program is ran on. <br />

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Open a terminal & use your choice of git clone to clone this repo. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo, but we want to be in the subfolder `as3`
- To get there lets do `cd CS-381/as3`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## Turn on Support for .JPG Files
There is an issue we need to address first: the program needs support for .jpg files enabled! <br />
- First, navigate to the as3 subfolder: `cd CS-381/as3`
- Now, we want to go into Raylibs config file and enable .JPG support.
- Lets navigate to the files location: `cd raylib-cpp/raylib/src`
- The `config.h` file for Raylib is in this folder. With your choice of text editor go ahead and open the file.
- For Linux: `gedit config.h` For macOS: `open -e config.h`
- Support for JPG files is on line 162. You can quickly find it using `Ctrl + Shift F` for Linux or `Command + F` on macOS.
- The line looks like this: `//#define SUPPORT_FILEFORMAT_JPG      1`
- Once you find this line, remove the leading comment marker (//) to enable support.
- Save the changes to the file and you're ready to move to the next step!

## How to Compile AS3
The following steps will guide you through compiling the code for AS3. <br />
- Lets navigate to our folder if not already there: `cd CS-381/as3`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS3.

## How to Run AS3
The following steps will guide you through running the code for AS3. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd CS-381/as3/build`
- In our previous guide we compiled the code for AS3, which should have produced an executable called `as3`
- Lets run this executable! Type `./as3`
- Congratulations! You've ran AS3.

## Controls Enumeration for AS3
**Not Applicable** <br />

## Extra Credit
**N/A** <br />
