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
    yarp::os::BufferedPort<Bottle> blob_extractor_port; //todo make private
    RpcClient classifier_port; //todo make private

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
        else if(paramsList.get(0).asString() == "BottleLocatedWithConfidenceX")
        {
            std::string objectName = "Bottle";
            if(paramsList.size() > 1) objectName = paramsList.get(1).asString();
            double confidence = 0.8;
            if(paramsList.size() > 2) confidence = paramsList.get(2).asDouble();

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
        if (blob_extractor_port.getInputCount() < 1)
        {
            yError() << "No connection to object blob extractor module (lbpExtract)";
            return BT_FAILURE;
        }

        Bottle *pBlobs = blob_extractor_port.read(false);
        if (!pBlobs)
        {
            yError() << "No object detected";
            return BT_FAILURE;
        }

        if (pBlobs->size() < 0)
        {
            yError() << "No object detected";
            return BT_FAILURE;
        }

        if (classifier_port.getOutputCount() < 1)
        {
            yError() << "No connection to object classifier module (himrepClassifier)";
            return BT_FAILURE;
        }

        Bottle cmd,reply;
        cmd.addVocab(Vocab::encode("classify"));
        Bottle &options=cmd.addList();
        for (int i=0; i<pBlobs->size(); i++)
        {
            std::ostringstream tag;
            tag<<"blob_"<<i;
            Bottle &item=options.addList();
            item.addString(tag.str());
            item.addList()=*(pBlobs->get(i).asList());
        }

        classifier_port.write(cmd,reply);

        for (int i=0; i<pBlobs->size(); i++)
        {
            std::ostringstream tag;
            tag<<"blob_"<<i;
            if(!reply.check(tag.str()))
            {
                yError() << "Invalid answer from object classifier module (himrepClassifier)";
                return BT_FAILURE;
            }

            Bottle *objectList = reply.find(tag.str()).asList();
            if(!objectList->check(objectName))
            {
                yError() << objectName << "is missing in object database";
                return BT_FAILURE;
            }

            double objectScore = reply.find(objectName).asDouble();
            if(objectScore > confidence)
            {
                return BT_SUCCESS;
            }
        }

        yInfo() << objectName << "not found, or found with insufficient confidence";

        return BT_FAILURE;
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
    skill.blob_extractor_port.open("/metaconditions/BottleLocalization/blobs:i");
    skill.classifier_port.open("/metaconditions/BottleLocalization/classifier/rpc:o");
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
