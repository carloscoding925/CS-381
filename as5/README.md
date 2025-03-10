# Assignment 5 - Carlos Hernandez - CS 381

## How does the Selection Management System Work?
For this assignment, i've opted to use tab selection for selecting the model which the user wants to control. <br />
A variable `selected_entity` is initialized at 0, and every time the user presses **TAB** this number increases <br />
by one. The first couple presses of **TAB** correlate directly to the vector index of the model we want, but if <br />
for example we have 3 models, after 4 presses our `selected_entity` variable will be at 3 which is no longer a valid <br />
index. To get around this, when we set the selected entity value, we want to perform `(selected_entity + 1) % vector.size()`. <br />
Lets break this down: By performing **modulo** `%` on the value of selected entity by the vector size, we are ensuring we <br />
always get a value from 0 to the vector size - 1. You'll notice we add 1 to `selected_entity`. If we stay with the example of <br />
3 models in our vector, our array is from indices `[0-2]` and the .size() function for vectors will return `3`. To ensure we <br />
always get an index value in our `selected_entity` variable, we want to add 1 to the current value so that when we perform <br />
modulo we get a value between 0 and 2, which are valid indices in our vector. <br />

## Compare & Contrast Monolithic Entities & Ad Hoc
In Monolithic Entities, game entities all belong to a single struct or class which defines their characteristics, in <br />
contrast with Ad Hoc where entities do not need to belong to the same struct as their behavior can be added or modified <br />
as needed. Both of these architectures can become messy and hard to maintain if the game begins to requires many different <br />
types of entities.

## How to Fetch the Raylib Submodule
The following steps will guide you through pulling the repo from github and fetching the `raylib-cpp` submodule. <br />
- Open a terminal & use your choice of git clone to clone this repo. For this tutorial i'll be using git clone through https.
- `git clone https://github.com/carloscoding925/CS-381.git`
- This will pull the entire `CS-381` repo, but we want to be in the subfolder `as5`
- To get there lets do `cd CS-381/as5`
- If we do `ls` you'll see `raylib-cpp` is already here, but it is not quite ready to use.
- To initialize it, lets perform the following: `git submodule update --init --recursive`
- Congratulations! You now have `raylib-cpp` initialized. 

## How to Compile AS5
The following steps will guide you through compiling the code for AS5. <br />
- Lets navigate to our folder if not already there: `cd CS-381/as5`
- We want to create a build folder here, so lets run the following: `mkdir build`
- Lets go into our build folder so we can compile: `cd build`
- In here, we can run the following commands to compile: `cmake ..` and `make`
- Congratulations! You have successfully compiled the code for AS5.

## How to Run AS5
The following steps will guide you through running the code for AS5. <br />
- Lets navigate to the build folder of our project if youre not already there: `cd CS-381/as5/build`
- In our previous guide we compiled the code for AS5, which should have produced an executable called `as5`
- Lets run this executable! Type `./as5`
- Congratulations! You've ran AS5.

## Controls Enumeration for AS5
- **W**: Increases Velocity in the Models Current Direction
- **S**: Decreases the Models Velocity to 0
- **A**: Turns the Model Left (increases heading)
- **D**: Turns the Model Right (decreases heading)
- **SPACE**: Stops the Model Immediately (sets model velocity to 0)

- **TAB**: Changes model selection
