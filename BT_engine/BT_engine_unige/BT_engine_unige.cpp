 

#include <map>
#include <memory>

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>



#include <caml/mlvalues.h>

//#ifdef USE_BTCPP
//
#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/xml_parsing.h>
// for connection with Groot
#include <behaviortree_cpp/loggers/bt_cout_logger.h>
#include <behaviortree_cpp/loggers/bt_file_logger.h>
#include <behaviortree_cpp/loggers/bt_zmq_publisher.h>
#include <behaviortree_cpp/loggers/bt_minitrace_logger.h>
//#endif

#include "tinyXML2/tinyxml2.h"



/** Behavior Tree are used to create a logic to decide what
 * to "do" and when. For this reason, our main building blocks are
 * Actions and Conditions.
 *
 * In this tutorial, we will learn how to create custom ActionNodes.
 * It is important to remember that NodeTree are just a way to
 * invoke callbacks (called tick() ). These callbacks are implemented by the user.
 */

// clang-format off
static const char* xml_text = R"(

 <root main_tree_to_execute = "MainTree" >

  <BehaviorTree ID="MainTree">
     <ReactiveSequence name="root_sequence">
        <OpenGripper/>
        <CloseGripper/>
        <SyncBtCppClient/>
        <YARP_BtCppClient/>
     </ReactiveSequence>
  </BehaviorTree>

 </root>
 )";

/*
<CheckBattery   name="battery_ok"/>
<OpenGripper    name="open_gripper"/>
<ApproachObject name="approach_object"/>
<CloseGripper   name="close_gripper"/>
*/

// clang-format on

class GripperInterface
{
  public:
    GripperInterface()
    {
    }

    BT::NodeStatus open() {
        yInfo() << "GripperInterface::open return SUCCESS";
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus close() {
        yInfo() << "GripperInterface::close return SUCCESS";
        return BT::NodeStatus::SUCCESS;
    }
};

int main()
{
    yarp::os::Network init;

    // We use the BehaviorTreeFactory to register our custom nodes
    BT::Tree  tree_cpp;
    BT::BehaviorTreeFactory factory;
    std::map<std::string, BT::TreeNode::Ptr> nodeMap;


    /* There are two ways to register nodes:
    *    - statically, i.e. registering all the nodes one by one.
    *    - dynamically, loading the TreeNodes from a shared library (plugin).
    * */

#ifdef MANUAL_STATIC_LINKING
    // Note: the name used to register should be the same used in the XML.
    // Note that the same operations could be done using DummyNodes::RegisterNodes(factory)

    using namespace bt_cpp_modules;

    // The recommended way to create a Node is through inheritance.
    // Even if it requires more boilerplate, it allows you to use more functionalities
    // like ports (we will discuss this in future tutorials).
    factory.registerNodeType<SyncBtCppClient>("SyncBtCppClient");
    factory.registerNodeType<BtCppClient>("YARP_BtCppClient");

    // Registering a SimpleActionNode using a function pointer.
    // you may also use C++11 lambdas instead of std::bind
//    factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));


    //You can also create SimpleActionNodes using methods of a class
    GripperInterface gripper;
//    BtCppClient cl("ciao", {});
//    factory.registerSimpleAction("BtCppClient", std::bind(&BtCppClient::tick, &cl));
    factory.registerSimpleAction("OpenGripper", std::bind(&GripperInterface::open, &gripper));
    factory.registerSimpleAction("CloseGripper", std::bind(&GripperInterface::close, &gripper));


#else
    // Load dynamically a plugin and register the TreeNodes it contains
    // it automated the registering step.
    factory.registerFromPlugin("./libdummy_nodes_dyn.so");
#endif
    // Trees are created at deployment-time (i.e. at run-time, but only once at the beginning).
    // The currently supported format is XML.
    // IMPORTANT: when the object "tree" goes out of scope, all the TreeNodes are destroyed
    auto tree = factory.createTreeFromText(xml_text);

    // To "execute" a Tree you need to "tick" it.
    // The tick is propagated to the children based on the logic of the tree.
    // In this case, the entire sequence is executed, because all the children
    // of the Sequence return SUCCESS.
    for(auto i=0; i<10; i++)
    {
        yDebug() << "run " << i;
        yDebug() << "Starting with " << toStr(tree.root_node->status());

        NodeStatus ret = tree.root_node->executeTick();
        yDebug() << "Ended with " << toStr(ret);
        std::this_thread::sleep_for( std::chrono::milliseconds(5) );
    }


    int i{0};
    yDebug() << "start while loop " << i;

    while( tree.root_node->executeTick() == NodeStatus::RUNNING)
    {
        yDebug() << "while " << ++i;
        std::this_thread::sleep_for( std::chrono::milliseconds(10) );
    }

    std::this_thread::sleep_for( std::chrono::seconds(1) );
    yDebug() << "Ended with " << toStr(tree.root_node->status());
    return 0;
}

/* Expected output:
*
       [ Battery: OK ]
       GripperInterface::open
       ApproachObject: approach_object
       GripperInterface::close
*/
