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
#include <BTMonitorMsg.h>
#include <yarp/os/PortablePair.h>

using namespace yarp::os;
using namespace yarp::sig;

class PourDrink : public TickServer,  public RFModule
{
private:

    RpcClient action_module_port; // default name /PourDrink/action/rpc:o, should be connected to /action-gateway/cmd:io
    RpcClient blackboard_port; // default name /PourDrink/blackboard/rpc:o,  should be connected to /blackboard/rpc:i
    RpcClient reaching_calibration_port; // default name /PourDrink/reachingCalibration/rpc:o,  should be connected to /iolReachingCalibration/rpc
    Port toMonitor_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        this->set_status(BT_RUNNING);

        Bottle paramsList;
        paramsList.fromString(params);

        std::string targetName("Glass");
        if(paramsList.size() > 0)
        {
            std::string target = paramsList.get(0).asString();
            if(target != "")
            {
                targetName = target;
            }
            else
            {
                yError() << "execute_tick: invalid first parameter. Should be a string (target name).";
            }
        }

        //connects to the blackboard to retrieve the glass top position in ref frame

        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "no connection to blackboard";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        Bottle cmd;
        cmd.addString("get");
        cmd.addString(targetName+"Shape");

        Bottle reply;
        blackboard_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "invalid answer from blackboard module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }
        else    // if we got a response from blackboard we can say e_req was sent
        {
            // send message to monitor: we are doing stuff
            BTMonitorMsg msg;
            msg.skill     = getName();
            msg.event     = "e_req";
            toMonitor_port.write(msg);
        }

        Bottle *vector = reply.get(0).asList();
        if(vector->size() < 10)
        {
            yError() << "invalid glass shape vector retrieved from the blackboard: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "Received target properties:" << vector->toString();

        Vector targetCenter(3);
        Vector targetOrientation(4);
        for(int i=0 ; i<3 ; i++) targetCenter[i] = vector->get(i).asDouble();
        for(int i=0 ; i<4 ; i++) targetOrientation[i] = vector->get(3+i).asDouble();

        Matrix targetRot = yarp::math::axis2dcm(targetOrientation);

        int axis=2;
        if(fabs(targetRot(2,0)) > fabs(targetRot(2,2)))
        {
            if(fabs(targetRot(2,0)) > fabs(targetRot(2,1))) axis = 0;
            else axis = 1;
        }
        else if(fabs(targetRot(2,1)) > fabs(targetRot(2,2))) axis = 1;
        else axis = 2;

        Vector upAxis = targetRot.subcol(0,axis,3);
        if(upAxis[2] < 0) upAxis = -1*upAxis;

        Vector targetTopPosition = targetCenter + vector->get(7+axis).asDouble() * upAxis;

        yInfo() << "Pouring target computed:" << targetTopPosition.toString();

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        //connects to the reaching calibration module to fix the position

        Vector targetTopPositionFixed = targetTopPosition;

        if(reaching_calibration_port.getOutputCount()>0)
        {
            cmd.clear();
            cmd.addString("get_location_nolook");
            cmd.addString("iol-right");
            cmd.addDouble(targetTopPosition[0]);
            cmd.addDouble(targetTopPosition[1]);
            cmd.addDouble(targetTopPosition[2]);
            cmd.addInt(0);

            reply.clear();
            reaching_calibration_port.write(cmd, reply);

            if (reply.size() < 4)
            {
                yError() << "invalid answer from reaching calibration module: " << reply.toString();
                this->set_status(BT_FAILURE);
                return BT_FAILURE;
            }
            else if (reply.get(0).asVocab() == Vocab::encode("ok"))
            {
                targetTopPositionFixed[0] = reply.get(1).asDouble();
                targetTopPositionFixed[1] = reply.get(2).asDouble();
                targetTopPositionFixed[2] = reply.get(3).asDouble();
            }
        }
        else
        {
            yWarning() << "no connection to reaching calibration module";
        }

        yInfo() << "Pouring target fixed for kinematics:" << targetTopPositionFixed.toString();

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

        yInfo() << "Retrieved bottle neck offset:" << bottleNeckOffset.toString();

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
        Bottle &sourceList = cmd.addList();
        sourceList.addString("source");
        for (int i=0 ; i<3 ; i++) sourceList.addDouble(bottleNeckOffset[i]);
        Bottle &destinationList = cmd.addList();
        destinationList.addString("destination");
        for (int i=0 ; i<3 ; i++) destinationList.addDouble(targetTopPositionFixed[i]);

        yInfo() << "Sending to action module:" << cmd.toString();

        reply.clear();
        action_module_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "invalid answer from pouring module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "pouring failed: see output of pouring module for more information";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "Pouring success";

        //connects to the action module to drop the bottle

        cmd.clear();
        cmd.addVocab(Vocab::encode("drop"));

        reply.clear();
        action_module_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "invalid answer from action module: " << reply.toString();
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "dropping failed: see output of action module for more information";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        yInfo() << "Dropping success";

        //connects to the blackboard to set the ContentPoured flag

        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "no connection to blackboard module";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        cmd.clear();
        cmd.addString("set");
        cmd.addString("ContentPoured");
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

        yInfo() << "ContentPoured set to True in the blackboard";

        {
        // send message to monitor: we are done with it
        yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
        BTMonitorMsg &msg = monitor.head;
        msg = monitor.head;
        msg.skill     = getName();
        msg.event     = "e_from_env";
        monitor.body.addString(params);
        toMonitor_port.write(monitor);
        }

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

        std::string reaching_calibration_port_name= "/"+this->getName()+"/reachingCalibration/rpc:o";
        if (!reaching_calibration_port.open(reaching_calibration_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << reaching_calibration_port_name;
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
        reaching_calibration_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        action_module_port.close();
        blackboard_port.close();
        reaching_calibration_port.close();

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
