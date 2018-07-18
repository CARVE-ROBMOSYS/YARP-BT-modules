# README #
YARP package for Behavior Tree's leaf nodes (Actions and Conditions).


### What is this repository for? ###

This package contains the C++ classes that your YARP RFModule should inherith. It also contains two small examples: one for an action node and one for a condition node. 

### How do I get set up? ###

Download and build the repository 

```console
git clone https://github.com/CARVE-ROBMOSYS/YARP-BT-modules
cd YARP-BT-modules
mkdir build
cd build
cmake ..
make -j
```

### Set up a Behavior Tree's action in YARP
The file examples/action_example_module.cpp is an example on how the your YARP BT action should look like, it performs an action (it is an action in the Behavior Tree).
Your action is a thrift server and does stuff. The Behavior Tree is a client and tells to the Server when and if the Server have to start (Tick) and which have to stop (Halt).

in your module you should implement two functions: `int Tick()` and `void Halt()`


In the function bool Tick() you must write the code to be executed when the module needs to be run.
The function it must return BT_SUCCESS if the execution of the action has succeeded and BT_FAILURE if it has failed.
To allow preemption of your action, it is preferable to check whenever possible if the action has been halted checking the function is_halted().

For example:


    int Tick()
    {
        if (!is_halted())
        {
            std::cout << "Doing Something" << std::endl;
        }
        return BT_SUCCESS;
    }


In the function Halt() you must write the code to be executed when the module needs to be stopped (e.g. when stopping a walking module we would like to have to robot stop in a home position).
For Example:

    void Halt()
    {
            std::cout << "Halting the Action" << std::endl;
    }

### Set up a Behavior Tree's condition in YARP
TODO


### Test your YARP action or condition
TODO


### Responsible ###

Michele Colledanchise michele.colledanchise@iit.it
