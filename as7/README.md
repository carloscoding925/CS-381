# Assignment 7 - Carlos Hernandez - CS 381

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Open a terminal & use your choice of git clone to clone this repo. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo, but we want to be in the subfolder `as7`
- To get there lets do `cd CS-381/as7`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## How to Compile AS7
The following steps will guide you through compiling the code for AS7. <br />
- Lets navigate to our folder if not already there: `cd CS-381/as7`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS7.

## How to Run AS7
The following steps will guide you through running the code for AS7. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd CS-381/as7/build`
- In our previous guide we compiled the code for AS7, which should have produced an executable called `as7`
- Lets run this executable! Type `./as7`
- Congratulations! You've ran AS7.

## Controls Enumeration for AS7
- **W**: Increases Velocity in the Models Current Direction
- **S**: Decreases the Models Velocity to 0
- **A**: Turns the Model Left (increases heading)
- **D**: Turns the Model Right (decreases heading)
- **SPACE**: Stops the Model Immediately (sets model velocity to 0)

- **TAB**: Changes model selection
