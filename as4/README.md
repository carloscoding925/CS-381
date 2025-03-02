# Assignment 4 - Carlos Hernandez - CS 381

## The Rizzler & Big AJ Present: Skibidi Toilet Glizzy Gobbler
Welcome! <br />
Our game is simple. **YOU** are Skibidi Toilet! Navigate around the terrain and collect as many hot dogs as <br />
you can! The instructions below will walk you through how to run the game on your system and what the controls <br />
are. <br />

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Open a terminal & use your choice of git clone to clone this repo. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo, but we want to be in the subfolder `as4`
- To get there lets do `cd CS-381/as4`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## How to Compile AS4
The following steps will guide you through compiling the code for AS4. <br />
- Lets navigate to our folder if not already there: `cd CS-381/as4`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS4.

## How to Run AS4
The following steps will guide you through running the code for AS4. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd CS-381/as4/build`
- In our previous guide we compiled the code for AS4, which should have produced an executable called `as4`
- Lets run this executable! Type `./as4`
- Congratulations! You've ran AS4.

## Controls Enumeration for AS4
- **W**: HOLD down W to travel in the direction the toilet is facing
- **S**: HOLD down S to travel in reverse
- **A**: HOLD down A to turn left
- **D**: HOLD down D to turn right
- **SPACE**: Jump

- **T**: Display/Hide High Score
- **V**: Display/Hide Volume Control

## Extra Credit
The following are things I added for extra credit <br />
- Free 10 Points (+10 points in rubric)
- Terminal ASCII Art (costco guys and chillguy)
- Points system (Press T to see high score and glizzy counter on top left, high score saved to a file as well for persistence)
- (Objectively) funny name