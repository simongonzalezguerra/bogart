# bogart
Bogart is a simple graphic application that showcases graphics development with C++14 and Horde3D.

It shows a 3D visualization of the Sponza model using the Horde3D library for rendering and GLFW for window management and user input. The project showcases the following concepts:

* Asynchronous programming with message passing
* The Model-View-Controller pattern
* Rendering and game logic in separate threads

The project is implemented in C++14 and uses the following features:

* The `<thread>` library
* The `<chrono>` library
* Smart pointers with `unique_ptr`
* Lambda
* Move capture
* Lambda capture expressions

# Building on Linux

0. In order to build you'll need a g++-4.9 for the C++14 features. In Ubuntu 14.04 you can install it with the following commands:
  0. `$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test`
  0. `$ sudo apt-get update`
  0. `$ sudo apt-get install g++-4.9`
0. Download the code or clone the repo
0. Cd to the directory where you downloaded the code
0. Build makefiles
   `$ mkdir build`
   `$ cd build`
   `$ cmake ..`
0. Build the code
   `$ make`

# Running

0. Run bogart
  `$ ./run.sh`
0. Use W, A, S and D to move and the mouse to look around.
0. You can also run the tests with the runTestChicago.sh and runTestTimers1.sh scripts