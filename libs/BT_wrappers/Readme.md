## How to use the YARP Tick Wrappers

![](/doc/basic_BT_wrappers.png)

#### Client side
When the BT_engine is used, there is usually no need to create new clients. 
In case a custom client is required or the engine cannot be used, the instantiation of a client can be done as follow.

```
    TickClient client;
    client.configure_TickClient("/TickClient", "Test 1");
```

To use a TickClient we simply have to create one,  configure it by giving a `<Port Prefix Name>` like `"/TickClient"` 
and a `<client name>` like `"Test 1"`. The TickClient will open a port with name `<Port Prefix Name> + / + <client name> + /tick:o`
<br> **NOTE:** names can contain spaces, they will be replaced with '_' for the port name.

Then we connect the client to the TickServer
```
    client.connect("/TickServer/Test_1"));
```
    
After the connection is established, we can ask the TickServer to start its job by ticking it.<br>
The tick request has a mandatory parameters, which is the ActionID <action> and an optional parameter Property <params>.

```
    client.request_tick(ActionID action, yarp::os::Property params);  // parameters are optional
```

The ActionID is a small structure usedful to uniquely identify the client requesting the action, so that the server can safely  handle multiple clients at the same time. 
By using the provided BT_engine, this field is automatically filled. 

The fieds are:
```
    std::string target;
    std::string resources;
    std::int32_t action_ID;
```

`target:` this identify the main target the action is referred to. For example for a navigatin action, the target is the location to reach. <br>
`resources:` this identify which resources the action will need to use, for example for a grasping action this may indicate to perform the movement with left or right arm. <br>
`action_ID:` an ID number which is required to be unique for each Behaviour Tree for each node. In case the same action node is repeated more then once, each instance needs to have a different ID. <br>
For example the two leaf nodes "Go to Kitchen" and action "Go To living room" may be implemented as a single client 
in which the target is different; in this case also the `action_ID` number has to be different.

The Property <params> can contain any addictional parameter required by the server to perform the action. The Property is a dictionary type container able to store any data type.

#### Server side

To implement a server, a dedicated class has to be created by inheriting from TickServer:

`class MySkillClass : public TickServer` 

and then the methods `request_tick`, request_halt have to be implemented. 
The return value is the enum `ReturnStatus`; the halt function shall return either `BT_HALTED` or `BT_ERROR` in case a error occurs.

The routine `request_tick` shall not takes time to execute and return immediately a value. In case more time is required, 
then it has to be executed in a separated thread. In order to do so, when configuring the TickServer simply set the <threaded>
flag to true, like in the following example.

`skill.configure_TickServer("/TickServer", "Test 1", `**true**`);`

**NOTE:** The `request_tick` function will be executed only when it is necessary and for each target independently. 
This means:
- If a `request_tick` is already running for a target, no other calls to request_tick will be executed for the same target, 
  until the previous execution terminates.
- Different `request_tick` calls for different targets can run in parallel. All the relevant values like status, parameters etc are 
  handled independently so the implementation does not have to to worry about.
  
  
In case your server requires to store some custom data separately for each target, the `TickServer_withData<T>` can be used instead.
This class is an extension of TickServer with an addictional member `std::map userData<ActionID, T>` 
In this way, in the `request_tick` implementation, the map can be used to store and retrieve custom data by using the `ActionID`.
  
For example, in case it is required to store an integer counter: 
```
class MySkillClass : public TickServer_withData<int>
{
... // no need to declare the int, we will use the userData map already defined.
  ReturnStatus request_tick(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params) override
  {
     // in case this is the first time this target is requested, initialize the data
     if (userData.find(target) == userData.end())
       userData[target] = 0;
  
     userData[target]++;  // increment the counter
     // print it
     yInfo() << " counter now at " << userData[target] << " for target " << target.target;
  }
  
  ...
}
 ```
 
**NOTE:** The function `request_halt` is blocking and waiting for the corresponding `request_tick` to terminate the execution before returning. This is to ensure that when the `request_halt` returns, the module is really halted; the 
drawback is that, in case the `request_tick` or `request_halt` takes time to terminate, the execution of the behaviour tree is freezed until they exit.
It is important therefore to verify if a halt has been requested and terminate as soon as possible. For example:
``` 
request_tick(...)
{
...
  while(work_to_do || !isHaltRequested(target))
  {
    // do my work here
  }
  ...
}
```
The function `isHaltRequested(target)` will return true if and only if a halt request has been received for the specified target. If a halt has been requested for a different target, it'll return false and the execution will proceed.


#### BlackBoard

An importante piece of the infrastructure is the shared blackboard. It is a process working as a memory shared between 
all the other process running and the Behavior Tree engine.
When nodes needs to share information between them or a node implementation requires more parameter to correctly perform
its task, these information can be stored in and retrieved from the blackboard.

The memory is accessible as a dictionary `<key, value>` where the key is a `std::string` and the value is a `yarp::os::Property`. The `Property` is YARP-based dictionary-like container, in such a way the maximum flexibility can be achieved in storing custom data.
In conjunction with the YARP BT wrappers, the `key` of the blackboard is meant to be the `Action_ID target` described before.

This allows a TickClient/Server to get the parameters from the blackboard or to write updated information using information already available and well known to the user.

In order to communicate with the blackboard, a `BlackBoardClient` has to instantiate and configured, like shown:
```
   m_blackboardClient.configureBlackBoardClient("", getName());
   m_blackboardClient.connectToBlackBoard(remoteBB_name);
```

Then data can be get/set by using the corresponding functions:
```
    Property datum;
    datum.put("Grasped", Value(true));
    m_blackboardClient.setData("myTarget", datum);

    Property values = m_blackboardClient.getData("myTarget");
```

**NOTE:** When setting data to the BlackBoard, the new information will be merged with existing ones, adding new field if not present. This means there is no need to get the data, change it and set it back.
Image there are two nodes that manipulates data relative to the same target, like `FindObject` and `GraspObject` modules

```
    Property firstField;
    firstField.put("Found", Value(true));
    m_blackboardClient.setData("Bottle", datum);
```
The blackboard will now have an entry `target` Bottle with the field `Found` set to `true`.

```
    Property datum;
    datum.put("Grasped", Value(true));
    m_blackboardClient.setData("Bottle", datum);
```
After this new `setData` the `target` Bottle will *also* contain the addictional field `Grasped` set to `true`.




