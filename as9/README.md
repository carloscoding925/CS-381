# Assignment 9 - Carlos Hernandez - CS 381

## Plant Simulator
Welcome to Plant Simulator! You are a plant, and your quest is to collect as much biomass as you possibly <br />
can. Move around the map and collect as many broccolis as possible! <br />

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Open a terminal & use your choice of git clone to clone this repo. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo, but we want to be in the subfolder `as9`
- To get there lets do `cd CS-381/as9`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## How to Compile AS9
The following steps will guide you through compiling the code for AS9. <br />
- Lets navigate to our folder if not already there: `cd CS-381/as9`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS9.

## How to Run AS9
The following steps will guide you through running the code for AS9. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd CS-381/as9/build`
- In our previous guide we compiled the code for AS9, which should have produced an executable called `as9`
- Lets run this executable! Type `./as9`
- Congratulations! You've ran AS9.

## Controls Enumeration for AS9
- **W**: Increase forward speed by 40
- **S**: Decrease forward speed by 40, Increase Backwards speed by 40
- **A**: Turn left by 30 degrees
- **D**: Turn right by 30 degrees
