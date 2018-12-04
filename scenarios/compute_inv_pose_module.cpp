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
using namespace yarp::dev;

class ComputeInvPose : public TickServer
{
private:
    Bottle cmd, response;
public:
    yarp::os::Port blackboard_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        set_status(BT_RUNNING);

        yInfo() << "[ComputeInvPose] Action started";
        std::string inv_pose = "123456";
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPose");
        cmd.addString(inv_pose);
        blackboard_port.write(cmd,response);
        yInfo() << "ComputeInvPose InvPose is set to" << inv_pose;
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


    yarp::os::ResourceFinder rf;

    rf.configure(argc, argv);

    ComputeInvPose skill;
    skill.configure_tick_server("/ComputeInvPose");
    skill.blackboard_port.open("/ComputeInvPose/blackboard/rpc:o");
    // initialize blackboard

    skill.runModule(rf);

    return 0;
}
