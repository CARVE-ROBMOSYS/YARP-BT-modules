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

class BlackBoard : public RFModule, public TickServer
{

private:
    std::map<std::string, std::string> bb;
    yarp::os::Port blackboard_port; // a bb port to handle messages

public:
    ReturnStatus request_tick(const std::string& params = "") override
    {
        ReturnStatus ret;

        if (get(params) == "True")
        {
            ret = BT_SUCCESS;
        }
        else if (get(params) == "False")
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

    bool set(std::string key, std::string value)
    {
        bb[key] = value;
    }

    std::string  get(std::string key)
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

            std::string key, value;

            key  = command.get(1).asString();
            value = command.get(2).asString();
            set(key,value);
            reply.addInt(1);//TODO add sanity check here

        }
        else if (command.get(0).asString() == "get")
        {
            //request to get a value from the blackboard

            std::string key, value;

            key  = command.get(1).asString();
            value = get(key);
            reply.addString(value);
        }
        else
        {
            yError("[Blackboard module] Command %s not recognized", command.get(0).asString().c_str());
            return false;
        }

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

    blackboard.set("BottleGrasped", "False");
    blackboard.set("InvPoseComputed", "False");
    blackboard.set("InvPoseValid", "False");
    blackboard.set("InvPose", "False");


    blackboard.runModule(rf);


//    while (true)
//    {
//        std::this_thread::sleep_for( std::chrono::seconds(10));
//        yInfo() << "Running";
//    }
    return 0;
}
