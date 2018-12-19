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

class ComputeInvPose : public TickServer, public RFModule
{
private:
    Bottle cmd, response;
public:
    yarp::os::Port blackboard_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        set_status(BT_RUNNING);
        bool ret = false;
        yInfo() << "[ComputeInvPose] Action started";
        std::string inv_pose = "map_2rooms 2.0 3.0 -180.0";
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPose");
        cmd.addString(inv_pose);
        ret = blackboard_port.write(cmd,response);
        yInfo() << "[ComputeInvPose] InvPose is set to" << inv_pose << " ret value " << ret;

        // writing on the blackboard that the inv pose is computed (to make the condition "is InvPoseComputed" simple)
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPoseComputed");
        cmd.addString("True");
        ret = blackboard_port.write(cmd,response);
        yInfo() << "[InvPoseComputed] InvPose is set to True" << " ret value " << ret;

        // writing on the blackboard that the inv pose is valid (very bold assumption)
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPoseValid");
        cmd.addString("True");
        ret = blackboard_port.write(cmd,response);
        yInfo() << "[InvPoseValid] InvPose is set to True" << " ret value " << ret;

        set_status(BT_SUCCESS);
        return BT_SUCCESS;
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

    ComputeInvPose skill;
    skill.configure_tick_server("/ComputeInvPose");
    skill.blackboard_port.open("/ComputeInvPose/blackboard/rpc:o");
    skill.runModule(rf);

    return 0;
}
