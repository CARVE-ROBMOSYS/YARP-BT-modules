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
**NOTE:** The function Halt() is blocking. Hence you should put here the piece of code you want to execute before continuing the execution of the Behavior Tree.
          However, the code in Tick() (between one is_halted() checkpoint and another) is still being executed in another thread. In some cases, we do not want to waste time waiting for the code to reach
          the next checkpoint (e.g. the action is writing in a port that nobody is reading) but is some other cases, we do need to wait for the code to reach the next checkpoint (e.g. the action is writing commands to a motor).
          If we want to wait for the code to reach the next checkpoint, just add the following while loop in the function Halt().


        while (is_running())
        {
            std::cout << "The Action is still running, waiting for the code to reach the next checkpoint" << std::endl;
            // a sleep here could be helpful
        }

        
If not, do not add the while loop.
Then set a name for your module. The name has the be unique. It will be used bt the behavior tree to recognize it. For example if you created a class called MyBTModule

     MyActionModule* action_module = new MyActionModule("MyBTAction");


### Set up a Behavior Tree's condition in YARP
The procedure is similar to the one for the action node, with the only difference that you don't need to implement the function Halt().

The file examples/action_example_module.cpp gives an example of a BT condiiton node in YARP.

    

### Test your YARP action or condition

You can test your action or condition by running the module and calling the rpc services `request_tick()`  and `request_halt()`, the latter for actions only. 
The port name is `/<module_name>/cmd` (e.g. for the module abovem the port is /MyBTAction/cmd)


### Responsible ###

Michele Colledanchise michele.colledanchise@iit.it
