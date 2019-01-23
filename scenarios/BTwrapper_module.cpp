/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file BTwrapper_module.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 * @brief Behavior Tree leaf node for forwarding commands to the rpc port of another module.
 * @remarks Parameters must be specified via command line or configuration file:
 *          --wrapped_module_name: name of the wrapped module (prefix of the rpc port, without first "/") (mandatory)
 *          --wrapped_rpc_port_name: name of the rpc port of the wrapped module (without module prefix nor first "/") (optional, default "rpc:i")
 *          --ack_string: string sent back by the wrapped module in case of success  (optional, default "ack")
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

class BTwrapper : public TickServer,  public RFModule
{
private:

    std::string wrapped_module_name;
    std::string wrapped_rpc_port_name_full;
    std::string ack_string;
    RpcClient wrapper_rpc_port; // default name /<module_name>_wrapper/rpc:o, is connected automatically to the rpc port of the wrapped module (like /module_name/rpc:i)

public:
    ReturnStatus execute_tick(const std::string& command = "") override
    {
        this->set_status(BT_RUNNING);

        //connects to the wrapped module to send the command
        if(wrapper_rpc_port.getOutputCount()<1)
        {
            yError() << "no connection to external module" << wrapped_module_name;
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
            yError() << "missing command to forward to" << wrapped_rpc_port_name_full;
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "sending command to" << wrapped_rpc_port_name_full << ":" << command;

        Bottle cmd;
        cmd.fromString(command);

        Bottle reply;
        wrapper_rpc_port.write(cmd, reply);

        yInfo() << "received reply:" << reply.toString();

        if(reply.size() < 1)
        {
            yError() << "empty reply from" << wrapped_module_name;
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode(ack_string))
        {
            yError() << "received failure from" << wrapped_module_name << ":" << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        this->set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        if(!rf.check("wrapped_module_name"))
        {
            yError() << "missing name of the wrapped module in configuration";
            this->set_status(BT_FAILURE);
            return false;
        }

        wrapped_module_name = rf.find("wrapped_module_name").asString();

        this->setName((wrapped_module_name+"_BTwrapper").c_str());
        this->configure_tick_server("/"+this->getName(), true);

        wrapped_rpc_port_name_full= "/"+wrapped_module_name+"/"+rf.check("wrapped_rpc_port_name", Value("rpc:i")).asString();

        ack_string = rf.check("ack_string", Value("ack")).asString();

        std::string wrapper_rpc_port_name= "/"+this->getName()+"/rpc:o";
        if (!wrapper_rpc_port.open(wrapper_rpc_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << wrapper_rpc_port_name;
           this->set_status(BT_FAILURE);
           return false;
        }

        if(!Network::connect(wrapper_rpc_port_name, wrapped_rpc_port_name_full))
        {
            yError() << this->getName() << ": Unable to establish connection from" << wrapper_rpc_port_name << "to" << wrapped_rpc_port_name_full;
            this->set_status(BT_FAILURE);
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
        wrapper_rpc_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        wrapper_rpc_port.close();

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

    BTwrapper skill;

    return skill.runModule(rf);
}
