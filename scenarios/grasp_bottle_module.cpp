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
#include <yarp/math/Math.h>

//behavior trees imports
#include <include/tick_server.h>

using namespace yarp::os;
using namespace yarp::sig;

class GraspBottle : public TickServer,  public RFModule
{
private:

    RpcClient grasp_module_port; // default name /GraspBottle/grasping/rpc:o, should be connected to /graspProcessor/cmd:rpc
    RpcClient grasp_module_start_halt_port; // default name /GraspBottle/graspingStartHalt/rpc:o should be connected to /graspProcessor/cmd:rpc (optional)
    RpcClient blackboard_port; // default name /GraspBottle/blackboard/rpc:o,  should be connected to /blackboard/rpc:i

public:
    ReturnStatus execute_tick(const std::string& objectName = "") override
    {
        this->set_status(BT_RUNNING);

        std::string object;
        if(objectName == "") object = "Bottle";
        else object = objectName;

        //connects to the blackboard to retrieve the bottle position
        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "no connection to blackboard";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        // restart the grasping-module if it was previously stopped

        if(grasp_module_start_halt_port.getOutputCount()>0)
        {
            Bottle cmdStartHalt, replyStartHalt;
            cmdStartHalt.addString("restart");
            grasp_module_start_halt_port.write(cmdStartHalt, replyStartHalt);
        }

        //connects to the grasping module to perform the grasping

        if(grasp_module_port.getOutputCount()<1)
        {
            yError() << "no connection to grasping module";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "Start grasping process of" << object;

        Bottle cmd;
        cmd.addString("grasp");
        cmd.addString(object);
        cmd.addString("right");

        std::future<Bottle> future = std::async(std::launch::async, [this, cmd]{
                Bottle replyLocal;
                grasp_module_port.write(cmd, replyLocal);
                return replyLocal;
            });

        std::future_status status;
        do
        {
            status = future.wait_for(std::chrono::milliseconds(10));

            // waiting for execute_grasp_thread to finish and stop it if necessary
            if(this->getHalted())
            {
                Bottle cmdStartHalt, replyStartHalt;
                cmdStartHalt.addString("halt");
                grasp_module_start_halt_port.write(cmdStartHalt, replyStartHalt);
                future.wait();
                this->set_status(BT_HALTED);
                return BT_HALTED;
            }
        }
        while(status != std::future_status::ready);

        Bottle reply = future.get();

        if(reply.size() != 1)
        {
            yError() << "invalid answer from grasping module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "grasping failed: see output of grasping module for more information";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "grasping success";

        //connects to the blackboard: set the ObjectGrasped flag

        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "setObjectGraspedFlag: no connection to blackboard module";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        cmd.clear();
        cmd.addString("set");
        cmd.addString(object+"Grasped");
        cmd.addString("True");

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

        yInfo() << object+"Grasped written to blackboard";

        this->set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("GraspBottle");

        this->configure_tick_server("/"+this->getName(), true);

        std::string grasp_module_port_name= "/"+this->getName()+"/grasping/rpc:o";
        if (!grasp_module_port.open(grasp_module_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << grasp_module_port_name;
           return false;
        }

        std::string grasp_module_start_halt_port_name= "/"+this->getName()+"/graspingStartHalt/rpc:o";
        if (!grasp_module_start_halt_port.open(grasp_module_start_halt_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << grasp_module_start_halt_port_name;
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
        grasp_module_port.interrupt();
        grasp_module_start_halt_port.interrupt();
        blackboard_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        grasp_module_port.close();
        grasp_module_start_halt_port.close();
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

    GraspBottle skill;

    return skill.runModule(rf);
}
