/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file action_command_module.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 * @brief Behavior Tree leaf node for forwarding commands to the action-gateway.
 * @remarks This module requires the action-gateway module from r1-grasping repository.
 */

//standard imports
#include <cstdlib>
#include <string>

//YARP imports
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>

//behavior trees imports
#include <include/tick_server.h>

using namespace yarp::os;
using namespace yarp::sig;

class ActionCommand : public TickServer,  public RFModule
{
private:

    RpcClient action_module_port; // default name /ActionCommand/action/rpc:o, should be connected to /action-gateway/cmd:io
    RpcClient blackboard_port; // default name /ActionCommand/blackboard/rpc:o,  should be connected to /blackboard/rpc:i

public:
    ReturnStatus execute_tick(const std::string& command = "") override
    {
        this->set_status(BT_RUNNING);

        //connects to action-gateway to send the command
        if(action_module_port.getOutputCount()<1)
        {
            yError() << "no connection to action-gateway";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        //send command

        if(command == "")
        {
            yError() << "missing command for iol module";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "sending command to action-gateway" << command;

        Bottle cmd;
        cmd.fromString(command);

        Bottle reply;
        action_module_port.write(cmd, reply);

        yInfo() << "received reply:" << reply.toString();

        if(reply.size() != 1)
        {
            yError() << "invalid answer from action-gateway module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "action-gateway command failed:" << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        this->set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("ActionCommand");

        this->configure_tick_server("/"+this->getName(), true);

        std::string action_module_port_name= "/"+this->getName()+"/action/rpc:o";
        if (!action_module_port.open(action_module_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << action_module_port_name;
           return false;
        }

        std::string blackboard_port_name= "/"+this->getName()+"/blackboard/rpc:o";
        if (!blackboard_port.open(blackboard_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << blackboard_port_name;
           return false;
        }

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
        action_module_port.interrupt();
        blackboard_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        action_module_port.close();
        blackboard_port.close();

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

    ActionCommand skill;

    return skill.runModule(rf);
}
