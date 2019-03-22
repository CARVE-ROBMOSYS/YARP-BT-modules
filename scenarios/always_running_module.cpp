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

class AlwaysRunning : public TickServer
{
    Port toMonitor_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        set_status(BT_RUNNING);

        // send message to monitor: we are done with it
        yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
        BTMonitorMsg &msg = monitor.head;
        msg = monitor.head;
        msg.source    = "AlwaysRunning";
        msg.target    = "none";
        msg.event     = "e_req";
        monitor.body.addString(params);
        toMonitor_port.write(monitor);

        yInfo() << "[AlwaysRunning] Action started";
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

    AlwaysRunning skill;
    skill.configure_tick_server("/AlwaysRunning");

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
