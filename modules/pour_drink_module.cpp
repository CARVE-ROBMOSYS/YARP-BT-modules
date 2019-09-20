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
#include <iostream>
#include <chrono>
#include <random>

//YARP imports
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcClient.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
#include <yarp/BT_wrappers/blackboard_client.h>

//#include <BTMonitorMsg.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::BT_wrappers;

class PourDrink : public TickServer,  public RFModule
{
private:

    RpcClient action_module_port;           // default name /PourDrink/action/rpc:o, should be connected to /action-gateway/cmd:io
    RpcClient reaching_calibration_port;    // default name /PourDrink/reachingCalibration/rpc:o,  should be connected to /iolReachingCalibration/rpc
    Port      toMonitor_port;

    BlackBoardClient m_blackboardClient;

    const int objectShape_size = 10;
    const int bottleNeckOffset_size = 3;

public:
    bool request_initialize() override
    {
        if(action_module_port.getOutputCount()<1)
        {
            yError() << "no connection to action module";
            return false;
        }
        return true;
    }

    bool request_terminate()  override
    {
        return true;
    }

    ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        // ///////////////////////////////////////////
        // Check input target for valid parameters
        // ///////////////////////////////////////////
        if(target.target == "")
        {
            yError() << "Required target is missing!";
            return BT_ERROR;
        }

        //
        // Get object shape from params
        //
        Vector objectShape;
        Value &objectVal = params.find("shape");
        if(objectVal.isNull())
        {
            yError() << "missing object <shape> parameter. Cannot proceed.";
            return BT_FAILURE;
        }

        Property::copyPortable(objectVal, objectShape);
        if(objectShape.size() < objectShape_size)
        {
            yError() << "parameter <shape> has wrong lenght: expected " << objectShape_size << " elements, got " << objectShape.size();
            return BT_FAILURE;
        }

        //
        // Get BottleNeckOffset from params
        //
        Vector bottleNeckOffset;
        Value &bottleNeckOffsetVal = params.find("bottleNeckOffset");
        if(bottleNeckOffsetVal.isNull())
        {
            yError() << "missing object <shape> parameter. Cannot proceed.";
            return BT_FAILURE;
        }

        Property::copyPortable(bottleNeckOffsetVal, bottleNeckOffset);
        if(bottleNeckOffset.size() < bottleNeckOffset_size)
        {
            yError() << "parameter <bottleNeckOffset> has wrong lenght: expected " << bottleNeckOffset_size << " elements, got " << bottleNeckOffset.size();
            return BT_FAILURE;
        }
        yInfo() << "Retrieved bottle neck offset:" << bottleNeckOffset.toString();


        // if we got data correctly we can say e_req was sent (?? can we ??)
        {
/*
            // send message to monitor: we are doing stuff
            BTMonitorMsg msg;
            msg.skill     = getName();
            msg.event     = "e_req";
            toMonitor_port.write(msg);
*/
        }

        yInfo() << "Received target properties:" << objectShape.toString();

        Vector targetCenter(3);
        Vector targetOrientation(4);
        for(int i=0 ; i<3 ; i++) targetCenter[i] = objectShape[i];
        for(int i=0 ; i<4 ; i++) targetOrientation[i] = objectShape[3+i];

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

        // vector = objectShape
        Vector targetTopPosition = targetCenter + objectShape[7+axis] * upAxis;

        yInfo() << "Pouring target computed:" << targetTopPosition.toString();

        if(this->isHaltRequested(target))
        {
            return BT_HALTED;
        }

        //connects to the reaching calibration module to fix the position
        Vector targetTopPositionFixed = targetTopPosition;

        Bottle cmd, reply;
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

        if(this->isHaltRequested(target))
        {
            return BT_HALTED;
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
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "pouring failed: see output of pouring module for more information";
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
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "dropping failed: see output of action module for more information";
            return BT_FAILURE;
        }

        yInfo() << "Dropping success";

        Property p;
        p.put("ContentPoured", true);
        if(!m_blackboardClient.setData(target.target, p))
        {
            yError() << "error writing to blackboard.";
            return BT_FAILURE;
        }

        yInfo() << "ContentPoured set to True in the blackboard";

/*
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
*/
        return BT_SUCCESS;
    }

    ReturnStatus request_halt(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        return BT_HALTED;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("PourDrink");

        this->configure_TickServer("", this->getName(), true);

        std::string action_module_port_name= "/"+this->getName()+"/action/rpc:o";
        if (!action_module_port.open(action_module_port_name))
        {
           yError() << this->getName() << ": Unable to open port " << action_module_port_name;
           return false;
        }

        std::string remoteBB_name =  rf.check("blackboard_port", Value("/blackboard"), "Port prefix for remote BlackBoard module").toString();
        m_blackboardClient.configureBlackBoardClient("", "pour_drink");
        m_blackboardClient.connectToBlackBoard(remoteBB_name);

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
        reaching_calibration_port.interrupt();
        return true;
    }

    /****************************************************************/
    bool close() override
    {
        action_module_port.close();
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
