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
#include <iostream>                 // for std::cout
#include <chrono>

//YARP imports
#include <yarp/os/Network.h>        // for yarp::os::Network
#include <yarp/os/LogStream.h>      // for yError()
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IEncoders.h>

//behavior trees imports
#include <include/tick_server.h>


using namespace yarp::os;
using namespace std;

class SetInvPoseInvalid : public TickServer
{
private:
    Bottle cmd, response;
public:
    yarp::os::Port blackboard_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        set_status(BT_RUNNING);

        yInfo() << "[GotoInvPose] Action started";
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPoseValid");
        cmd.addString("False");
        blackboard_port.write(cmd,response);
        set_status(BT_SUCCESS);
        return BT_SUCCESS;
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

    SetInvPoseInvalid skill;
    skill.configure_tick_server("/setInvPoseInvalid");
    skill.blackboard_port.open("/setInvPoseInvalid/blackboard/rpc:o");

    /*
        std::cout << "Action ready. To send commands to the action, open and type: yarp rpc /setInvPoseInvalid/tick:i,"
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
