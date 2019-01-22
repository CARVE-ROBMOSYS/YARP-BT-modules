/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file pour_module.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 * @brief Behavior Tree leaf node for performing the pouring of a bottle into a glass.
 * @remarks This module requires the action-gateway from r1-grasping repository.
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
#include <yarp/math/Math.h>

//behavior trees imports
#include <include/tick_server.h>

using namespace yarp::os;
using namespace yarp::sig;

class PourDrink : public TickServer,  public RFModule
{
private:

    RpcClient action_module_port; // default name /PourDrink/action/rpc:o, should be connected to /action-gateway/cmd:io
    RpcClient blackboard_port; // default name /pourDrink/blackboard/rpc:o,  should be connected to /blackboard/rpc:i

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        this->set_status(BT_RUNNING);

        //connects to the blackboard to retrieve the glass top position in ref frame

        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "no connection to blackboard";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        Bottle cmd;
        cmd.addString("get");
        cmd.addString("GlassTopPosition");

        Bottle reply;
        blackboard_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "invalid answer from blackboard module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        Bottle *vector = reply.get(0).asList();
        if(vector->size() < 3)
        {
            yError() << "invalid glass top position vector retrieved from the blackboard: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        Vector glassTopPosition(3);
        for(int i=0 ; i<3 ; i++) glassTopPosition[i] = vector->get(i).asDouble();

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        //connects to the blackboard to retrieve the bottle neck position in the grasper frame

        cmd.clear();
        cmd.addString("get");
        cmd.addString("BottleNeckOffset");

        reply.clear();
        blackboard_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "invalid answer from blackboard module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        vector = reply.get(0).asList();
        if(vector->size() < 3)
        {
            yError() << "invalid bottle neck position vector retrieved from the blackboard: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        Vector bottleNeckOffset(3);
        for(int i=0 ; i<3 ; i++) bottleNeckOffset[i] = vector->get(i).asDouble();

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        //connects to the action module to perform the pouring

        if(action_module_port.getOutputCount()<1)
        {
            yError() << "no connection to action module";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        cmd.clear();
        cmd.addVocab(Vocab::encode("pour"));
        Bottle &targetList = cmd.addList();
        targetList.addString("target");
        for (int i=0 ; i<3 ; i++) targetList.addDouble(glassTopPosition[i]);
        Bottle &bottleNeckList = cmd.addList();
        bottleNeckList.addString("neck_position");
        for (int i=0 ; i<3 ; i++) bottleNeckList.addDouble(bottleNeckOffset[i]);

        reply.clear();
        action_module_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "invalid answer from grasping module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "pouring failed: see output of pouring module for more information";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "pouring success";

        //connects to the blackboard to set the PouringDone flag

        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "no connection to blackboard module";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        cmd.clear();
        cmd.addString("set");
        cmd.addString("PouringDone");
        cmd.addString("True");

        reply.clear();
        blackboard_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "invalid answer from blackboard module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asInt() != 1)
        {
            yError() << "invalid answer from blackboard module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "PouringDone set to True in the blackboard";

        this->set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("PourDrink");

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

    PourDrink skill;

    return skill.runModule(rf);
}
