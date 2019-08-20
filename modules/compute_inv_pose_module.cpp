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

//TBR
#include <thread>
#include <chrono>

//YARP imports
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
#include <yarp/BT_wrappers/blackboard_client.h>

//#include <BTMonitorMsg.h>

using namespace yarp::os;
using namespace yarp::BT_wrappers;

class ComputeInvPose : public TickServer, public RFModule
{
private:
    bool simulated{false};
//    Bottle cmd, response;
    BlackBoardClient    m_blackboardClient;
    Port toMonitor_port;

public:

    virtual ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {})
    {
        bool ret = false;

/*      TODO: monitoring messages
        if(ret)
        {
            // send message to monitor: we are doing stuff
            BTMonitorMsg msg;
            msg.skill    = getName();
            msg.event     = "e_req";
            toMonitor_port.write(msg);
        }
*/

        Property data;
        Property &loc = data.addGroup("location");
        loc.put("map_id", "sanquirico");
        if(simulated)   // gazebo simulated map
        {
            loc.put("x",     10.45);
            loc.put("y",      1.95);
            loc.put("theta",  0.0);
        }
        else    // real environment
        {
            loc.put("x",     11.19);
            loc.put("y",      1.84);
            loc.put("theta",  0.0);
        }
        data.put("computed", true);
        data.put("valid", true);
        data.put("robotRadius", Value(0.1));
        data.put("obstacle_avoidance", false);

        ret = m_blackboardClient.setData("invPose", data);
        yInfo() << "[ComputeInvPose] invPose is set to" << data.toString() << " ret value " << ret;


/*      TODO: monitoring messages
        if(ret)
        {
            // send message to monitor: we are done
            BTMonitorMsg msg;
            msg.skill    = getName();
            msg.event     = "e_from_env";
            toMonitor_port.write(msg);
        }
*/
        return BT_SUCCESS;
    }

    ReturnStatus request_halt(const ActionID &target, const yarp::os::Property &params = {})
    {
        yDebug() << "[ComputeInvPose] request_halt. Nothing to do.";
        return BT_HALTED;
    }

    bool configure(ResourceFinder &rf) override
    {
        this->configure_TickServer("", this->getName(), false);

        if(rf.check("sim"))
            simulated = true;

        // to connect to relative monitor
        toMonitor_port.open("/"+getName()+"/monitor:o");

        std::string remoteBB_name =  rf.check("blackboard_port", Value("/blackboard"), "Port prefix for remote BlackBoard module").toString();
        m_blackboardClient.configureBlackBoardClient("", getName());
        m_blackboardClient.connectToBlackBoard(remoteBB_name);
        return true;
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
    skill.setName("ComputeInvPose");
    skill.runModule(rf);
    return 0;
}
