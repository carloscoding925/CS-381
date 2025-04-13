# Assignment 8 - Carlos Hernandez - CS 381

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Open a terminal & use your choice of git clone to clone this repo. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo, but we want to be in the subfolder `as8`
- To get there lets do `cd CS-381/as8`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## How to Compile AS8
The following steps will guide you through compiling the code for AS8. <br />
- Lets navigate to our folder if not already there: `cd CS-381/as8`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS8.

## How to Run AS8
The following steps will guide you through running the code for AS8. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd CS-381/as8/build`
- In our previous guide we compiled the code for AS8, which should have produced an executable called `as8`
- Lets run this executable! Type `./as8`
- Congratulations! You've ran AS8.

## Controls Enumeration for AS8
- **W**: Speed up Car/Rocket by an incrementing value
- **S**: Decrease Car/Rocket speed by an incrementing value
- **A**: Increases entity heading/yaw
- **D**: Decrease entity heading/yaw
- **R**: Increase entity pitch
- **F**: Decrease entity pitch
- **Q**: Increase entity roll
- **E**: Decrease entity roll
- **SPACE**: Reset entity velocity to 0