## Integration with BehaviorTree_CPP library

This section is about the integration of YARP TickClient/TickServer with the [BehaviorTree.CPP](https://github.com/BehaviorTree/BehaviorTree.CPP) library.
The library is used to build the engine capable of reading BT description from XML file 
(more detail on XML format [here](https://www.behaviortree.dev/xml_format/) )
and properly send tick/halt signals and elaborate the results. The library provides also a log to file mechanism and a real-time
GUI for monitoring the BT execution called [Groot](https://github.com/BehaviorTree/Groot).

In order to create a new node for the BehaviorTree.CPP library, it has to inherit from either `BT::ActionNodeBase` if the node
is an action or from `BT::ConditionNode` if the node is a condition.

A set of basic nodes are already provided by this library, they should be enough for most cases.

#### YARP_tick_client

This is an action node embedding a YARP TickClient. It provides inputs parameters for:
  - `<serverPort>` [mandatory] : the YARP port of the TickServer it has to connect to (without the final `/tick:o`).
  - `<target>` [optional] : the target parameter for the TickServer, if any.
  - `<resources>` [optional] : the resources list for the TickServer, if any.
  
  In case a target is provided, the node will reach the shared blackboard to get any parameter associated with that 
  target and will forward them to the TickServer.
  
  So, to 'tick' any YARP module implemented using the `TickServer`, no code needs to be written, simply add a YARP_tick_client
  in the XML and set the correct `<serverPort>` like in the example hereafter:
  
``` 
  <Action ID="YARP_tick_client" name="Compute Inv Pose"  serverPort="/ComputeInvPose" target="InvPose"/>
```

#### Check condition

This node connects to the shared blackboard and check whether or not a boolean flag is `true` or `false`.
The input parameters are:
  - `<serverPort>` [mandatory] : the YARP port of the TickServer it has to connect to (without the final `/tick:o`).
  - `<target>` [mandatory] : the target parameter the flag is referred to; it is the key in the blackboard map
  - `<flag>` [mandatory] : the flag to be checked
  
  For example to check if the bottle is found, the target is `bottle` and the flag is `found`.
  
#### Set_Condition / Reset_Condition
Similar to the check condition, these action nodes will set the flag to true (for Set_Condition) or to false 
(for Reset_condition) when they receive the `tick` signal.

#### Where is my Robot?

In case special conditions needs to be checked and a dedicated perception module is available, it is better to create a 
condition node that directly connects to the perception and get the required answer.
An example of this architecture is done by the conditions `RobotInRoom` and `RobotAtLocation`. These nodes internally open
a yarp::dev::navigationClient and connects to the YARP navigation server to ask the current precise location of the robot
and verify if the required condition is satisfied. This solution is more efficient and is not affected by concurrency problem
which may arise between nodes an the blackboard.

