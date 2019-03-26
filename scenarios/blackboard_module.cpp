/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file condition_example_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

//standard imports
#include <iostream>             // for std::cout
#include <chrono>

//YARP imports
#include <yarp/os/Network.h>    // for yarp::os::Network
#include <yarp/os/LogStream.h>  // for yError()

//behavior trees imports
#include <include/tick_server.h>

#include <map> // the blackboard is a c++ map

using namespace yarp::os;

class BlackBoard : public TickServer, public RFModule
{

private:
    std::map<std::string, Value> bb;
    yarp::os::Port blackboard_port; // a bb port to handle messages

public:
    ReturnStatus request_tick(const std::string& params = "") override
    {
        ReturnStatus ret;

        if (get(params).asString() == "True")
        {
            ret = BT_SUCCESS;
        }
        else if (get(params).asString() == "False")
        {
            ret = BT_FAILURE;
        }
        else ret = BT_ERROR;

        ReturnStatusVocab a;
        yInfo() << "Request_tick got  params " << params << " replying with " << a.toString((int)ret);
        //std::this_thread::sleep_for( std::chrono::seconds(3));
        set_status(ret);
        return ret;
    }

    ReturnStatus request_status()
    {
        ReturnStatus ret = BT_RUNNING;
        ReturnStatusVocab a;
        yInfo() << "request_status, replying with " << a.toString(ret);
        return ret;
    }

    ReturnStatus request_halt()
    {
        ReturnStatus ret = BT_HALTED;
        ReturnStatusVocab a;
        yInfo() << "request_halt,  replying with " << a.toString(ret);
        return ret;
    }

    bool set(std::string key, Value value)
    {
        bb[key] = value;
    }

    Value  get(std::string key)
    {
        return bb[key];
    }

    double getPeriod()
    {
        // module periodicity (seconds), called implicitly by the module.
        return 1.0;
    }
    // This is our main function. Will be called periodically every getPeriod() seconds
    bool updateModule()
    {
        //cout << "[" << count << "]" << " updateModule..." << endl;
        return true;
    }
    // Message handler. Just echo all received messages.
    bool respond(const Bottle& command, Bottle& reply)
    {
        if (command.get(0).asString() == "set")
        {
            //request to set a value on the blackboard
            std::string key;

            key  = command.get(1).asString();
            Value value = command.get(2);
            set(key,value);
            reply.addInt(1);//TODO add sanity check here

        }
        else if (command.get(0).asString() == "get")
        {
            //request to get a value from the blackboard

            std::string key;
            Value value;

            key  = command.get(1).asString();
            value = get(key);

            reply.add(value);
        }
        else if (command.get(0).asString() == "help")
        {
            reply.addVocab(Vocab::encode("many"));
            reply.add(Value("Accepted commands are: "));
            reply.add(Value("set <entryName> <entryValue>  -- entries are stored as yarp::os::Value"));
            reply.addString("get <entryName>  -- entries are returned as yarp::os::Value");
            reply.addString("list   -- prints all stored entries");
            reply.addString("clear  -- erase  all entries");
            reply.addString("initialize  -- reset blackboard to its default state");
        }
        else if (command.get(0).asString() == "list")
        {
            reply.addVocab(Vocab::encode("many"));
            for(auto entry : bb)
            {
                reply.addString(entry.first + " : " + entry.second.toString() + "   ");
            }
        }
        else if (command.get(0).asString() == "clear")
        {
            bb.clear();
            reply.addInt(1);//TODO add sanity check here
        }
        else if (command.get(0).asString() == "initialize")
        {
            bb.clear();
            initializeValues();
            reply.addInt(1);//TODO add sanity check here
        }
        else
        {
            yError("[Blackboard module] Command %s not recognized. Call 'help' for a list of supported commands", command.get(0).asString().c_str());
            return false;
        }

        std::cout << "reply" << reply.toString() << std::endl;

        return true;
    }

    bool configure(yarp::os::ResourceFinder &rf)
       {
           // optional, attach a port to the module
           // so that messages received from the port are redirected
           // to the respond method
           blackboard_port.open("/blackboard/rpc:i");
           attach(blackboard_port);
           return true;
       }

    bool interruptModule()
    {
        return true;
    }
    // Close function, to perform cleanup.
    bool close()
    {
        // optional, close port explicitly
        blackboard_port.close();
        return true;
    }

    void initializeValues()
    {
        // initialize blackboard
        set("RoomKnown", Value("False"));
        set("RobotAtRoom", Value("False"));
        set("BottleFound", Value("False"));
        set("Room_navParams", Value("0.35 True"));
        set("Room", Value("sanquirico 9.2 1.9 71.0"));

        set("RobotAtFindBottle", Value("False"));
        set("FindBottle_navParams", Value("0.35 True"));
        set("FindBottle", Value("sanquirico 10.0 2.02 -14.56"));

        set("RobotAtKitchen", Value("False"));
        set("Kitchen_navParams", Value("0.35 True"));
        set("Kitchen", Value("sanquirico 10.0 2.02 0.0"));
        set("BottleLocated", Value("False"));
        set("BottleGrasped", Value("False"));
        set("InvPoseComputed", Value("False"));
        set("InvPoseValid", Value("False"));
        set("InvPose", Value("0.0 0.0 0.0"));
        set("InvPose_navParams", Value("0.1 False"));
        set("RobotAtInvPose", Value("False"));

        set("homeArms",                 Value("joint ( "
                                              "(left_arm  45 60 10 80 18) "
                                              "(right_arm 45 60 10 80 18) "
                                              "(torso 0.01) "
                                              ") dummy_blackboard_condition"));

        // Values for simulation may be different wrt the real world, so let's use
        // a dedicated set of parameters with <Sim> suffix

        // Just enter in the kitchen
        set("KitchenSim",                Value("sanquirico 10.0 2.02 90.0"));    // orientation is coherent to door entrance
        set("KitchenSim_navParams",      Value("0.35 True"));
        set("RobotAtKitchenSim",         Value("False"));

        // Rotate toward the table
        set("FindBottleSim",             Value("sanquirico 10.0 2.02 0.0"));     // robot oriented toward the table
        set("FindBottleSim_navParams",   Value("0.35 True"));
        set("RobotAtFindBottleSim",      Value("False"));

        set("InvPoseSim",                Value("sanquirico 10.9152 1.68453 -12.7319"));
        set("InvPoseSim_navParams",      Value("0.1 False"));
        set("RobotAtInvPoseSim",         Value("False"));

        set("grasp Bottle",              Value("joint ( (right_arm 28.8 37.35 -10.2 54.9 40) ) BottleGrasped"));
    }
};

int main(int argc, char * argv[])
{
    /* initialize yarp network */
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available!";
        return EXIT_FAILURE;
    }


    yarp::os::ResourceFinder rf;

    rf.configure(argc, argv);

    BlackBoard blackboard;
    blackboard.configure_tick_server("/blackboard");

    blackboard.initializeValues();

    blackboard.runModule(rf);
    return 0;
}
