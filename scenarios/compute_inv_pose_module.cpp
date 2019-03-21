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
#include <random>

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
using namespace yarp::dev;

class ComputeInvPose : public TickServer, public RFModule
{
private:
    Bottle cmd, response;
    yarp::os::Port blackboard_port;
    Port toMonitor_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        set_status(BT_RUNNING);
        bool ret = false;
        yInfo() << "[ComputeInvPose] Action started";

        static bool first = true;
        static bool toggle = true;

        double randAngle = 0;

        double randX =  11.19;
        /*
        // generate random number
        std::random_device rd;
        std::mt19937 gen(rd());
        double min_x{5}, max_x{+30};
        std::uniform_real_distribution<double> dis_theta(min_x, max_x);
        std::uniform_real_distribution<double> dis_x(0, 0.2);

        if(first)
        {
            yInfo() << "first";
            randAngle = 0;
            first     = false;
        }
        else
        {
            yInfo() << "toggle " << toggle;
            toggle ? randAngle = dis_theta(gen) : randAngle = -dis_theta(gen);
            toggle = !toggle;
        }
        // create string
        std::ostringstream strsX, strsTheta;
        std::string strX, strTheta;
        strsX << randX;
        strX= strsX.str();

        strsTheta << randAngle;
        strTheta = strsTheta.str();
        std::string inv_pose = "sanquirico 11.18 1.90 " + strTheta;
        */

        // without random
        std::string inv_pose = "sanquirico 11.18 1.90 0.0";


        if(blackboard_port.getOutputCount() > 0)
        {
            // send message to monitor: we are doing stuff
            yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
            BTMonitorMsg &msg = monitor.head;
            msg = monitor.head;
            msg.source    = getName();
            msg.target    = "blackboard";
            msg.event     = "e_req";
            monitor.body.addString(params);
            toMonitor_port.write(monitor);
        }

        // create command
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

        // Invalidating robot at inv pose
        cmd.clear();
        response.clear();
        cmd.addString("set");
        cmd.addString("RobotAtInvPose");
        cmd.addString("False");
        ret = blackboard_port.write(cmd,response);
        yInfo() << "[RobotAtInvPose] is set to False" << " ret value " << ret;


        {
        // send message to monitor: we are done
        yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
        BTMonitorMsg &msg = monitor.head;
        msg = monitor.head;
        msg.source    = "blackboard";
        msg.target    = getName();
        msg.event     = "e_from_env";
        toMonitor_port.write(monitor);
        }
        set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    bool configure(ResourceFinder &rf) override
    {
        this->configure_tick_server("/"+this->getName());
        blackboard_port.open("/"+this->getName() + "/blackboard/rpc:o");

        // to connect to relative monitor
        toMonitor_port.open("/"+this->getName()+"/monitor:o");
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
