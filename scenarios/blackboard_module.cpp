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

class BlackBoard : public TickServer
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


/*
    std::cout << "Action ready. To send commands to the action, open and type: yarp rpc /ActionExample/cmd,"
              <<" then type help to find the available commands "
              << std::endl;
*/

    // initialize blackboard

    blackboard.set("BottleGrasped", Value("False"));
    blackboard.set("InvPoseComputed", Value("False"));
    blackboard.set("InvPoseValid", Value("False"));
    blackboard.set("InvPose", Value("000000"));


    blackboard.runModule(rf);


//    while (true)
//    {
//        std::this_thread::sleep_for( std::chrono::seconds(10));
//        yInfo() << "Running";
//    }
    return 0;
}
