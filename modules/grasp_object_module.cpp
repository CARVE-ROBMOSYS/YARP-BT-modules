/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file grasp_bottle_module.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 * @brief Behavior Tree leaf node for performing the grasping of a bottle.
 * @remarks This module requires the grasp-processor from grasp-pose-gen repository.
 */

//standard imports
#include <cstdlib>
#include <string>
#include <future>
#include <thread>
#include <chrono>

//YARP imports
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/PortablePair.h>
//#include <yarp/math/Math.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
#include <yarp/BT_wrappers/blackboard_client.h>

//#include <BTMonitorMsg.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::BT_wrappers;

class GraspBottle : public TickServer,  public RFModule
{
private:

    RpcClient home_port;                    // to send arms in home position
    RpcClient grasp_module_port;            // default name /GraspBottle/grasping/rpc:o, should be connected to /graspProcessor/cmd:rpc
//    RpcClient grasp_module_start_halt_port; // default name /GraspBottle/graspingStartHalt/rpc:o should be connected to /graspProcessor/cmd:rpc (optional)

    BlackBoardClient m_blackboardClient;

    Port toMonitor_port;

    void halting()
    {
        Bottle cmdStartHalt, replyStartHalt;
        cmdStartHalt.addString("halt");
//        grasp_module_start_halt_port.write(cmdStartHalt, replyStartHalt);
    }

public:

    bool request_initialize() override
    {
        // Verify all required connections are up and running
        if(grasp_module_port.getOutputCount() < 1)
        {
            yError() << "no connection to grasping module...  remote port should be like </graspProcessor/cmd:rpc>";
            return false;
        }

//        if(grasp_module_start_halt_port.getOutputCount() < 1)
//        {
//            yError() << "no connection to grasp module Start/Halt port... ...  remote port should be like </graspProcessor/cmd:rpc>";
//            return false;
//        }
        return true;
    }

    bool request_terminate() override
    {
        halting();
        return true;
    }

    ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {}) override
    {

        yInfo() << "My own request_tick with \n target <" + target.target + "> \n resources <" + target.resources + ">\n params " << params.toString();
/*
        {
        // send message to monitor: we are doing stuff
        yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
        BTMonitorMsg &msg = monitor.head;
        msg = monitor.head;
        msg.skill     = getName();
        msg.event     = "e_req";
        toMonitor_port.write(monitor);
        }
*/

        if(target.target == "")
        {
            yError() << "<target> parameter is required.";
            return BT_ERROR;
        }


        if(target.target == "homeArms")
        {
            yarp::os::Bottle cmd;
            cmd.addString("home");
            cmd.addString("arms");
//            std::this_thread::sleep_for(std::chrono::seconds(2));
            bool ret = home_port.write(cmd);
            yInfo() << "\n\nhomeArms ret " << ret << " cmd " << cmd.toString();
            return BT_SUCCESS;
        }

        //
        // Check input target for valid parameters
        //

        // target != 'homeArms' are considered the object to grasp
        {
            if(target.resources.size() == 0)
            {
                yError() << "Required parameter is missing! Both target and resource are required.";
                return BT_ERROR;
            }

            if(! (target.resources == "right") || (target.resources == "left") )
            {
                yError() << "Resource value not valid. Choose between 'right' or 'left', got <" + target.resources + ">";
                return BT_ERROR;
            }

            if(this->isHaltRequested(target))
            {
                return request_halt(target, params);
            }

            // restart the grasping-module if it was previously stopped
            Bottle cmdStartHalt, replyStartHalt;
            cmdStartHalt.addString("restart");
            grasp_module_port.write(cmdStartHalt, replyStartHalt);

            yInfo() << "Start grasping process of" << target.target;

            // The following is a blocking call.
            // The tickServer is run in threaded mode, so this is safe to do.
            // Halting is handled by 'request_halt' callback.
            Bottle cmd;
            cmd.addString("grasp");
            cmd.addString(target.target);
            cmd.addString(target.resources);

            // When this call ends, check if prematurely ended due to a halt request
            if(this->isHaltRequested(target))
            {
                return BT_HALTED;
            }

            // If everything ended up as expected, then notify blackboard and
            // return Sucess/Failure
            Bottle reply;
            grasp_module_port.write(cmd, reply);

            if(reply.size() != 1)
            {
                yError() << "invalid answer from grasping module: " << reply.toString();
                return BT_FAILURE;
            }

            if(reply.get(0).asVocab() != Vocab::encode("ack"))
            {
                yError() << "grasping failed: see output of grasping module for more information";
                return BT_FAILURE;
            }

            yInfo() << "grasping success";

            // Notify the blackboard: set the <Object>Grasped flag
            // NOTE: this BB should be ideally the same where we get the input <params> from ...
            // How to do it?
            Property datum;
            datum.put("Grasped", Value(true));
            m_blackboardClient.setData(target.target, datum);

            if(reply.size() != 1)
            {
                yError() << "invalid answer from blackboard module: " << reply.toString();
                return BT_FAILURE;
            }

            if(reply.get(0).asInt() != 1)
            {
                yError() << "invalid answer from blackboard module: " << reply.toString();
                return BT_FAILURE;
            }

            yInfo() << target.target << "Grasped written to blackboard";
        }

/*      TBD: monitoring message ... probably it'll be handled outside this scope if possbile
        {
        // send message to monitor: we are done with it
        yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
        BTMonitorMsg &msg = monitor.head;
        msg = monitor.head;
        msg.skill     = getName();
        msg.event     = "e_from_env";
        toMonitor_port.write(monitor);
        }
*/
        return BT_SUCCESS;
    }

    ReturnStatus request_halt(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        yTrace() << "Grasp module received Halt request!";
        halting();

        // Notify the blackboard: set the <Object>Grasped flag
        // NOTE: this BB should be ideally the same where we get the input <params> from ...
        // How to do it?

        m_blackboardClient.setData(target.target, "(Grasped false)");
        return BT_HALTED;
    }


    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("GraspObject");

        this->configure_TickServer("", this->getName(), true);

        std::string grasp_module_port_name= "/"+this->getName()+"/grasping/rpc:o";
        if (!grasp_module_port.open(grasp_module_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << grasp_module_port_name;
           return false;
        }

//        std::string grasp_module_start_halt_port_name= "/"+this->getName()+"/graspingStartHalt/rpc:o";
//        if (!grasp_module_start_halt_port.open(grasp_module_start_halt_port_name))
//        {
//           yError() << this->getName() << ": Unable to open port " << grasp_module_start_halt_port_name;
//           return false;
//        }

        if (!m_blackboardClient.configureBlackBoardClient("", this->getName()))
        {
           return false;
        }

        std::string remoteBB_name =  rf.check("blackboard_port", Value("/blackboard"), "Port prefix for remote BlackBoard module").toString();
        m_blackboardClient.configureBlackBoardClient("", this->getName());
        m_blackboardClient.connectToBlackBoard(remoteBB_name);
        home_port.open("/"+this->getName()+"/home:c");
        if(!yarp::os::Network::connect(home_port.getName(), "/action-gateway/cmd:io"))
        {
            yError() << this->getName() << ": Unable to connect to action gateway.";
            return false;
        }
        if(!yarp::os::Network::connect(grasp_module_port.getName(), "/graspProcessor/cmd:rpc"))
        {
            yError() << this->getName() << ": Unable to connect to /graspProcessor/cmd:rpc.";
            return false;
        }

        // to connect to relative monitor
//        toMonitor_port.open("/"+this->getName()+"/monitor:o");
        return true;
    }

    /****************************************************************/
    double getPeriod() override
    {
        return 1.0;
    }

    /****************************************************************/
    bool updateModule() override
    {
        return true;
    }

    /****************************************************************/
    bool respond(const Bottle &command, Bottle &reply) override
    {

    }

    /****************************************************************/
    bool interruptModule() override
    {
        grasp_module_port.interrupt();
//        grasp_module_start_halt_port.interrupt();
        toMonitor_port.interrupt();
        return true;
    }

    /****************************************************************/
    bool close() override
    {
        grasp_module_port.close();
//        grasp_module_start_halt_port.close();
        toMonitor_port.close();
        return true;
    }
};

int main(int argc, char *argv[])
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

    GraspBottle skill;
    return skill.runModule(rf);
}
