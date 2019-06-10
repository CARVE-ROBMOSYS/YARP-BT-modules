/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file meta_conditions_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

//standard imports
#include <iostream>             // for std::cout
#include <chrono>

//YARP imports
#include <yarp/os/Network.h>    // for yarp::os::Network
#include <yarp/os/LogStream.h>  // for yError()
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/RpcClient.h>

//behavior trees imports
#include <include/tick_server.h>

using namespace yarp::os;

class MetaConditions : public TickServer
{
public:

    // blackboard
    yarp::os::Port blackboard_port; //todo make private

    // object localization
    RpcClient object_properties_collector_port; //todo make private

private:
    Bottle cmd, response;

public:
    ReturnStatus request_tick(const std::string& params = "") override
    {
        Bottle paramsList;
        paramsList.fromString(params);

        cmd.clear();
        response.clear();
        ReturnStatus ret;
        if(paramsList.get(0).asString() == "AtInvPose")
        {
            cmd.addString("get");
            cmd.addString("InvPose");
            blackboard_port.write(cmd,response);
            std::string inv_pose =  response.get(0).asString();
            yInfo() << "InvPose is" << inv_pose;
            // code to check if the robot is in a neigborhood of the inv_pose.
            // **TODO** read from the localization system the robot's positon  check if the robot is in a neigborhood of the inv_pose.
            yError()<< "Node not yet implemented";

            ret = BT_SUCCESS;
        }
        else if(paramsList.get(0).asString() == "ObjectLocatedWithConfidenceX")
        {
            std::string objectName = "Bottle";
            if(paramsList.size() > 1) objectName = paramsList.get(1).asString();
            else yWarning() << "ObjectLocatedWithConfidenceX called without object argument, will use \"Bottle\" as default";

            double confidence = 0.7;
            if(paramsList.size() > 2) confidence = paramsList.get(2).asDouble();
            else yWarning() << "ObjectLocatedWithConfidenceX called without confidence argument, will use" << confidence << "as default";

            ret = this->processObjectLocatedWithConfidenceX(objectName, confidence);
        }

// Not used, we will tick directly the blackboard


//        if(params == "BottleGrasped")
//        {
//            cmd.addString("get");
//            cmd.addString("BottleGrasped");
//            blackboard_port.write(cmd,response);
//            bool bottle_grasped =  response.get(0).asBool();
//            yInfo() << "BottleGrasped is" << bottle_grasped;
//            ret = bottle_grasped ? BT_SUCCESS : BT_FAILURE;
//        }

//        if(params == "InvPoseComputed")
//        {
//            cmd.addString("get");
//            cmd.addString("InvPoseComputed");
//            blackboard_port.write(cmd,response);
//            bool inv_pose_computed =  response.get(0).asBool();
//            yInfo() << "InvPoseComputed is" << inv_pose_computed;
//            ret = inv_pose_computed ? BT_SUCCESS : BT_FAILURE;
//        }

//        if(params == "InvPoseValid")
//        {
//            cmd.addString("get");
//            cmd.addString("InvPoseValid");
//            blackboard_port.write(cmd,response);
//            bool inv_pose_valid =  response.get(0).asBool();
//            yInfo() << "InvPoseValid is" << inv_pose_valid;
//            ret = inv_pose_valid ? BT_SUCCESS : BT_FAILURE;
//        }


        else
        {
            ret = BT_ERROR;
            yError()<< "Node "<< params << "not implemented";
        }
        ReturnStatusVocab a;
        yInfo() << "Request_tick got  params " << params << " replying with " << a.toString((int)ret);
        std::this_thread::sleep_for( std::chrono::seconds(3));
        return ret;
    }

    ReturnStatus request_status()
    {
        ReturnStatus ret = BT_RUNNING;
        ReturnStatusVocab a;
        yInfo() << "request_status, replying with " << a.toString(ret);
        return ret;
    }

    ReturnStatus request_halt()
    {
        ReturnStatus ret = BT_HALTED;
        ReturnStatusVocab a;
        yInfo() << "request_halt,  replying with " << a.toString(ret);
        return ret;
    }

    ReturnStatus processObjectLocatedWithConfidenceX(const std::string objectName= "Bottle", double confidence = 0.5)
    {
        if(object_properties_collector_port.getOutputCount()<1)
        {
            yError() << "No connection to objectPropertiesCollector module";
            return BT_FAILURE;
        }

        Bottle cmd;
        cmd.fromString("ask ((name == "+objectName+"))");

        Bottle reply;
        object_properties_collector_port.write(cmd, reply);

        if(reply.size() != 2)
        {
            yError() << "invalid answer to object id query from objectPropertiesCollector module: wrong size: " << reply.toString();
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "invalid answer to object id query from objectPropertiesCollector module: failed: " << reply.toString();
            return BT_FAILURE;
        }

        if(!reply.check("id"))
        {
            yError() << "invalid answer to object id query from objectPropertiesCollector module: missing id: " << reply.toString();
            return BT_FAILURE;
        }

        Bottle *objectIDlist = reply.find("id").asList();

        if(objectIDlist->size() < 1)
        {
            yError() << "object" << objectName << "does not exist in the database";
            return BT_FAILURE;
        }

        cmd.clear();
        cmd.fromString("get (id "+objectIDlist->get(0).toString()+")");

        object_properties_collector_port.write(cmd, reply);

        if(reply.size() != 2)
        {
            yError() << "invalid answer to object properties query from objectPropertiesCollector module: wrong size: " << reply.toString();
            return BT_FAILURE;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "invalid answer to object properties query from objectPropertiesCollector module: failed: " << reply.toString();
            return BT_FAILURE;
        }

        if(!reply.get(1).check("position_3d"))
        {
            yInfo() << objectName << "not found in the scene";
            return BT_FAILURE;
        }

        if(reply.get(1).check("class_score"))
        {
            double object_confidence = reply.get(1).find("class_score").asDouble();

            if(object_confidence < confidence)
            {
                yInfo() << objectName << "was found but confidence is insufficient";
                return BT_FAILURE;
            }
        }
        else
        {
            yError() << objectName << "was found but confidence measure is missing";
            return BT_FAILURE;
        }

        return BT_SUCCESS;
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

    MetaConditions skill;
    skill.configure_tick_server("/metaconditions");
    skill.blackboard_port.open("/metaconditions/blackboard/rpc:o");
    skill.object_properties_collector_port.open("/metaconditions/objectPropertiesCollector/rpc:o");
/*
    std::cout << "Action ready. To send commands to the action, open and type: yarp rpc /metaconditions/tick:i,"
              <<" then type help to find the available commands "
              << std::endl;
*/
    while (true)
    {
        std::this_thread::sleep_for( std::chrono::seconds(10));
        yInfo() << "Running";
    }
    return 0;
}
