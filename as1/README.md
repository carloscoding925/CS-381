# Assignment 1 - Carlos Hernandez - CS 381

## How Audio is Produced by a Speaker
In order to create sound, a simple speaker needs two components: a motor and a cone. In this setup, the cone <br />
is the dinner-plate shaped black plastic typically seen on most speakers or headphones. The cone is responsible <br />
for creating sound waves which translates to audio we can hear. The cone and motor are connected so as the motor <br /> 
vibrates, the cone converts these vibrations into air waves by pushing on the air. This brings us to the second piece of <br />
the speaker which is the motor. The motor is essentially an electromagnet which translates electrical signals into <br />
movement which is then translated by the cone into sound waves.

## Why is raylib::AudioDevice Necessary
`raylib::AudioDevice` is necessary in this code because it allows for easy management of audio in our program. <br />
By initializing an `AudioDevice` object we create an audio system which allows us to perform a variety of <br />
operations on audio files such as play them, change the volume, pause them, and other modifications to the <br />
audio stream. An `AudioDevice` object also handles management of audio resources for us so we can simply use <br />
functions like `LoadSound` and `UnloadSound` to automatically manage the storage of our audio files in our program. <br />

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Lets create a folder to store the project. Run the following commands:
- `mkdir ~/Documents/my-project`
- Lets go into our new folder: `cd ~/Documents/my-project`
- Now that we're here, lets initialize an empty git repo: `git init`
- Now, use your choice of git clone to clone this repo into your new folder. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo into your folder, but we want to be in the subfolder `as1`
- To get there lets do `cd CS-381/as1`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule init` and `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## How to Compile AS1
The following steps will guide you through compiling the code for AS1. <br />
- Lets navigate to our folder from the previous step if not already there: `cd ~/Documents/my-project/CS-381/as1`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS1.

## How to Run AS1
The following steps will guide you through compiling the code for AS1. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd ~/Documents/my-project/CS-381/as1/build`
- In our previous guide we compiled the code for AS1, which should have produced an executable called `as1`
- Lets run this executable! Type `./as1`
- Congratulations! You've ran AS1.

## How to Enumerate the Controls for AS1
The controls for AS1 are as follows: <br />
- **TAB**: Change selected slider
- **Right Arrow Key** Increase Volume
- **Left Arrow Key** Decrease Volume