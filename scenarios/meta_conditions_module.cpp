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

using namespace yarp::os;

class MetaConditions : public TickServer
{
public:
    yarp::os::Port blackboard_port; //todo make private

private:
    Bottle cmd, response;

public:
    ReturnStatus request_tick(const std::string& params = "") override
    {
        request.clear();
        response.clear();
        ReturnStatus ret;
        if(params == "AtInvPose")
        {


            cmd.addString("get");
            cmd.addString("InvPose");


            blackboard_port.write(cmd,response);


            std::string inv_pose =  response.get(0).asString();
            // code to check if the robot is in a neigborhood of the inv_pose.
            ret = BT_SUCCESS;
        }
        else if( params == "BottleLocated" )
        {
            //we assume is always located located
            ret = BT_SUCCESS;
        }
        else ret = BT_ERROR;
        ReturnStatusVocab a;
        yInfo() << "Request_tick got  params " << params << " replying with " << a.toString((int)ret);
        std::this_thread::sleep_for( std::chrono::seconds(3));
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

    MetaConditions skill;
    skill.configure_tick_server("/metaconditions");
    skill.blackboard_port.open("/metaconditions/blackboard/rpc:o");

/*
    std::cout << "Action ready. To send commands to the action, open and type: yarp rpc /ActionExample/cmd,"
              <<" then type help to find the available commands "
              << std::endl;
*/
    while (true)
    {
        std::this_thread::sleep_for( std::chrono::seconds(10));
        yInfo() << "Running";
    }
    return 0;
}
