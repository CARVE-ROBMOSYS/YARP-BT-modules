/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file iolStateMchineHandler_command_module.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 * @brief Behavior Tree leaf node for forwarding commands to the iolStateMachineHandler.
 * @remarks This module requires the iolStateMachineHandler from iol repository.
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

class iolStateMachineHandlerCommand : public TickServer,  public RFModule
{
private:

    RpcClient iol_module_port; // default name /iolStateMachineHandlerCommand/iol/rpc:o, should be connected to /iolStateMachineHandler/human:rpc
    RpcClient blackboard_port; // default name /iolStateMachineHandlerCommand/blackboard/rpc:o,  should be connected to /blackboard/rpc:i

public:
    ReturnStatus execute_tick(const std::string& command = "") override
    {
        this->set_status(BT_RUNNING);

        //connects to iolStateMachineHandler to send the command
        if(iol_module_port.getOutputCount()<1)
        {
            yError() << "no connection to iolStateMachineHandler";
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

        yInfo() << "sending command to iolStateMachineHandler" << command;

        Bottle cmd;
        cmd.fromString(command);

        Bottle reply;
        iol_module_port.write(cmd, reply);

        yInfo() << "received reply:" << reply.toString();

        if(reply.size() != 1)
        {
            yError() << "invalid answer from iolStateMachineHandler module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "iolStateMachineHandler command failed:" << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        this->set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("iolStateMachineHandlerCommand");

        this->configure_tick_server("/"+this->getName(), true);

        std::string iol_module_port_name= "/"+this->getName()+"/iol/rpc:o";
        if (!iol_module_port.open(iol_module_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << iol_module_port_name;
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
        iol_module_port.interrupt();
        blackboard_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        iol_module_port.close();
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

    iolStateMachineHandlerCommand skill;

    return skill.runModule(rf);
}
