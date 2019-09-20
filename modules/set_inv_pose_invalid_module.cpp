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
#include <iostream>
#include <chrono>
#include <random>

//YARP imports
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
#include <yarp/BT_wrappers/blackboard_client.h>

//#include <BTMonitorMsg.h>

using namespace yarp::os;
using namespace yarp::BT_wrappers;

class SetInvPoseInvalid : public TickServer
{
public:
    BlackBoardClient    blackboardClient;
    Port toMonitor_port;

public:

    bool request_initialize() override
    {
        return true;
    }

    bool request_terminate()  override
    {
        return true;
    }

    ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        yInfo() << "[Set invalid pose] Action started";

/*      TODO: monitoring messages
        if(ret)
        {
            // send message to monitor: we are doing stuff
            BTMonitorMsg msg;
            msg.skill     = "setInvPoseInvalid";
            msg.event     = "e_req";
            toMonitor_port.write(msg);
        }
*/

        Property data;
        data.put("valid", false);
        data.put("computed", false);
        blackboardClient.setData("InvPose", data);


/*      TODO: monitoring messages
        if(ret)
        {
            // send message to monitor: we are done
            yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
            BTMonitorMsg &msg = monitor.head;
            msg = monitor.head;
            msg.skill     = "setInvPoseInvalid";
            msg.event     = "e_from_env";
            toMonitor_port.write(monitor);
        }
*/
        // This module always returns running, as from @miccol specifications
        return BT_RUNNING;
    }

    ReturnStatus request_halt(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        return BT_HALTED;
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
    std::string moduleName = "SetInvPoseInvalid";
    skill.configure_TickServer("", moduleName, false);

    skill.blackboardClient.configureBlackBoardClient("", moduleName);
    skill.blackboardClient.connectToBlackBoard();

    skill.toMonitor_port.open("/"+moduleName+"/monitor:o");
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
