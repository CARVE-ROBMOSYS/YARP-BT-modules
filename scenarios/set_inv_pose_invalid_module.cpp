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
#include <BTMonitorMsg.h>
#include <yarp/os/PortablePair.h>


using namespace yarp::os;
using namespace std;

class SetInvPoseInvalid : public TickServer
{
private:
    Bottle cmd, response;
    Port toMonitor_port;

public:
    yarp::os::Port blackboard_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        set_status(BT_RUNNING);

        if(blackboard_port.getOutputCount() > 0)
        {
            // send message to monitor: we are doing stuff
            yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
            BTMonitorMsg &msg = monitor.head;
            msg = monitor.head;
            msg.source    = "setInvPoseInvalid";
            msg.target    = "blackboard";
            msg.event     = "e_req";
            monitor.body.addString(params);
            toMonitor_port.write(monitor);
        }

        yInfo() << "[Set invalid pose] Action started";
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPoseValid");
        cmd.addString("False");
        blackboard_port.write(cmd,response);

        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPoseComputed");
        cmd.addString("False");
        blackboard_port.write(cmd,response);

        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("InvPose");
        cmd.addString("nope 0 0 0");
        blackboard_port.write(cmd,response);
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("RobotAtInvPose");
        cmd.addString("False");
        blackboard_port.write(cmd,response);

        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("BottleLocated");
        cmd.addString("False");
        blackboard_port.write(cmd,response);

        {
        // send message to monitor: we are done
        yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
        BTMonitorMsg &msg = monitor.head;
        msg = monitor.head;
        msg.source    = "blackboard";
        msg.target    = "setInvPoseInvalid";
        msg.event     = "e_from_env";
        toMonitor_port.write(monitor);
        }

        // This module always returns running, as from @miccol specifications
        return BT_RUNNING;
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
