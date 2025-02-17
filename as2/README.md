# Assignment 2 - Carlos Hernandez - CS 381

## What is the point of the DrawBoundedModel function?
something here <br />

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Lets create a folder to store the project. Run the following commands:
- `mkdir ~/Documents/my-project`
- Lets go into our new folder: `cd ~/Documents/my-project`
- Now that we're here, lets initialize an empty git repo: `git init`
- Now, use your choice of git clone to clone this repo into your new folder. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo into your folder, but we want to be in the subfolder `as2`
- To get there lets do `cd CS-381/as2`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## How to Compile AS2
The following steps will guide you through compiling the code for AS2. <br />
- Lets navigate to our folder from the previous step if not already there: `cd ~/Documents/my-project/CS-381/as2`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS2.

## How to Run AS1
The following steps will guide you through compiling the code for AS2. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd ~/Documents/my-project/CS-381/as2/build`
- In our previous guide we compiled the code for AS2, which should have produced an executable called `as2`
- Lets run this executable! Type `./as2`
- Congratulations! You've ran AS2.

## Controls Enumeration for AS2
**NotApplicable** <br />

## Extra Credit
