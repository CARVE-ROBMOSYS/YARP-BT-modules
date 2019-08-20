# README #
YARP package for Behavior Tree's leaf nodes (Actions and Conditions) and engine.


### What is this repository for? ###

This package contains:

Libraries:
- Yarp Behaviour Tree wrapper: Client/server YARP wrapper to handle the network communication and propagate the `tick` from the engine to the leaf node actually implementing the core functionality of each node. Thrift powered.
- Behaviour Tree CPP leaves: a basic and flexible set of predefined BT leaf nodes compatible with [BehaviorTree.CPP](https://github.com/BehaviorTree/BehaviorTree.CPP) library.

Executables:
- Behavior Tree engine using [BehaviorTree.CPP](https://github.com/BehaviorTree/BehaviorTree.CPP) library and compatible with [Groot](https://github.com/BehaviorTree/Groot) GUI.
- Set of executable nodes handling common funtionalities of the robot, like navigation.
- Blackboard for sharing data between nodes and other parameters.

### How do I get set up? ###

Dependencies:

In order to implement new nodes, the only dependency required is [YARP](https://github.com/robotology/YARP).
If you wish to run the engine or create new a leaf node, then [BehaviorTree.CPP](https://github.com/BehaviorTree/BehaviorTree.CPP) library is required.

Download and build the repository 


```console
$ git clone https://github.com/CARVE-ROBMOSYS/YARP-BT-modules
$ cd YARP-BT-modules
$ mkdir build
$ cd build
$ cmake ..
$ make -j
```

If BehaviorTree.CPP library is found, corresponding nodes and the engine will automatically be compiled.
It is then required to add the `BT_CPP_PLUGIN_DIRS` environment variable pointing to the `lib` folder in the build or install path. This is required to load the node plugins at runtime.


### Quick glance at Behavior Trees in YARP

To run the Behavior Tree engine two parameters can be used:

- bt_description [required]: this is the name of the xml file containing the behavior tree description. It will be searched and loaded with the YARP resource finder, so the `--context` option can be used to better specify where to look for and the `--verbose` option will print all the paths the ResourceFinder is searching into.
- libraries [required for external plugins]: this is the name of the plugin library where the nodes have to be loaded.
For example this repository creates a library called `libBT_CPP_leaves.so` containing all the basic nodes. For more information they are described [here](https://github.com/barbalberto/YARP-BT-modules/tree/refactorPostCarve/libs/BT_CPP_leaves). This library is automatically added as a source of plugins, so there is no need to specify it, but 
if you need to load plugins created by a different repository, the name of that library is then required. The library 
will be searched in the paths contained in the `BT_CPP_PLUGIN_DIRS` environment variable. Note: the env var shall always point to the folder containing `libBT_CPP_leaves.so` since this library is always required.

`BT_engine_cpp --bt_description my_BT.xml --context my_working_context --libraries my_lib.so`

The Groot GUI if running, will show the graphical representation of the BT like in the following picture.

![](BT_example.png)

The engine will then start, instantiate all nodes and will sent tick signals to the node.

To understand how BT nodes communicate in YARP, see [YARP BT wrapper lib](https://github.com/barbalberto/YARP-BT-modules/tree/refactorPostCarve/libs/BT_wrappers)

Then to better understand the integration with BehaiorTree_CPP library and the engine, see [BT_CPP_leaves](https://github.com/barbalberto/YARP-BT-modules/tree/refactorPostCarve/libs/BT_CPP_leaves)

### Responsible ###

Michele Colledanchise michele.colledanchise@iit.it
