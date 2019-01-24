/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file locate_bottle_module.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 * @brief Behavior Tree leaf node for performing the localization of an object (a bottle by default).
 * @remarks This module requires the objectPropertiesCollector module from iol repository.
 */

//standard imports
#include <cstdlib>
#include <string>

//YARP imports
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

//behavior trees imports
#include <include/tick_server.h>

using namespace yarp::os;
using namespace yarp::sig;

/****************************************************************/
class LocateBottle : public RFModule, public TickServer
{
    RpcClient object_properties_collector_port; // should be connected to /memory/rpc
    RpcClient gaze_exploration_port; // should be connected to /iolStateMachineHandler/human:rpc
    RpcClient point_cloud_read_port; // should be connected to /pointCloudRead/rpc
    RpcClient find_superquadric_port; // should be connected to /find-superquadric/points:rpc
    RpcClient blackboard_port; // should be connected to /blackboard/rpc:i

    /****************************************************************/
    bool getObjectPosition(const std::string &objectName, Vector &position)
    {
        //connects to an object recognition database: sends the object name and retrieves object location

        position.resize(3, 0.0);

        if(object_properties_collector_port.getOutputCount()<1)
        {
            yError() << "getObjectPosition: no connection to objectPropertiesCollector module";
            return false;
        }

        Bottle cmd;
        cmd.fromString("ask ((name == "+objectName+"))");

        Bottle reply;
        object_properties_collector_port.write(cmd, reply);

        if(reply.size() != 2)
        {
            yError() << "getObjectPosition: invalid answer to object id query from objectPropertiesCollector module: " << reply.toString();
            return false;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "getObjectPosition: object" << objectName << "does not exist in the database";
            return false;
        }

        if(!reply.check("id"))
        {
            yError() << "getObjectPosition: invalid answer to object id query from objectPropertiesCollector module: missing id:" << reply.toString();
            return false;
        }

        Bottle *objectIDlist = reply.find("id").asList();

        if(objectIDlist->size() < 1)
        {
            yError() << "getObjectPosition: empty id list: object" << objectName << "does not exist in the database";
            return false;
        }

        cmd.clear();
        cmd.fromString("get ((id "+objectIDlist->get(0).toString()+") (propSet (position_3d)))");

        object_properties_collector_port.write(cmd, reply);

        if(reply.size() != 2)
        {
            yError() << "getObjectPosition: invalid answer to object position query from objectPropertiesCollector module: " << reply.toString();
            return false;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "getObjectPosition: invalid answer to object position query from objectPropertiesCollector module:" << objectName << "position not found: " << reply.toString();
            return false;
        }

        if(!reply.get(1).check("position_3d"))
        {
            yError() << "getObjectPosition: invalid answer to object position query from objectPropertiesCollector module:" << objectName << "position not found: " << reply.toString();
            return false;
        }

        Bottle *position_3d = reply.get(1).find("position_3d").asList();

        if(position_3d->size() < 3)
        {
            yError() << "getObjectPosition: invalid dimension of object position retrived from objectPropertiesCollector module: " << position_3d->toString();
            return false;
        }

        for(int i=0 ; i<3 ; i++) position[i] = position_3d->get(i).asDouble();

        yInfo() << "Retrieved" << objectName << "position:" << position.toString();

        return true;
    }

    /****************************************************************/
    bool getObjectShape(const std::string &objectName, Vector &objectShape)
    {
        //connects to point-cloud-read and find-superquadric to retrieve the position, orientation and size of the object from the object name
        //objectShape will be filled with (center_x center_y center_z rot_axis_x rot_axis_y rot_axis_z rot_angle size_axis_x size_axis_y size_axis_z)

        if(point_cloud_read_port.getOutputCount()<1)
        {
            yError() << "getObjectProperties: no connection to point-cloud-read module";
            return false;
        }

        if(find_superquadric_port.getOutputCount()<1)
        {
            yError() << "getObjectProperties: no connection to find-superquadric module";
            return false;
        }

        Bottle cmd;
        cmd.addString("get_point_cloud");
        cmd.addString(objectName);

        Bottle reply;
        point_cloud_read_port.write(cmd, reply);

        if(reply.size() < 1)
        {
            yError() << "getObjectProperties: empty reply from point-cloud-read:" << reply.toString();
            return false;
        }

        PointCloud<DataXYZRGBA> pointCloud;
        if(!pointCloud.fromBottle(*(reply.get(0).asList())))
        {
            yDebug() << "getObjectProperties: invalid reply from point-cloud-read:" << reply.toString();
            return false;
        }

        reply.clear();
        find_superquadric_port.write(pointCloud, reply);

        Vector objectSuperquadric;
        reply.write(objectSuperquadric);

        if (objectSuperquadric.size() != 9)
        {
            yError() << "getObjectProperties: invalid reply from find-superquadric:" << reply.toString();
            return false;
        }

        objectShape.resize(10);
        objectShape[0] = objectSuperquadric[0];
        objectShape[1] = objectSuperquadric[1];
        objectShape[2] = objectSuperquadric[2];
        objectShape[3] = 0.0;
        objectShape[4] = 0.0;
        objectShape[5] = 1.0;
        objectShape[6] = M_PI/180*objectSuperquadric[3];
        objectShape[7] = objectSuperquadric[4];
        objectShape[8] = objectSuperquadric[5];
        objectShape[9] = objectSuperquadric[6];

        yInfo() << "Retrieved" << objectName << "shape:" << objectShape.toString();

        return true;
    }

    bool writeObjectPositionToBlackboard(const std::string &objectName, Vector &position)
    {
        if(position.size() != 3)
        {
            yError() << "writeObjectPositionToBlackboard: invalid position vector dimension";
            return false;
        }

        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "writeObjectPositionToBlackboard: no connection to blackboard";
            return false;
        }

        Bottle cmd;
        cmd.addString("set");
        cmd.addString(objectName+"Pose");
        Bottle &subcmd = cmd.addList();
        for(int i=0 ; i<3 ; i++)
        {
            subcmd.addDouble(position[i]);
        }

        Bottle reply;
        blackboard_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "writeObjectPositionToBlackboard: invalid answer from blackboard: " << reply.toString();
            return false;
        }

        if(reply.get(0).asInt() != 1)
        {
            yError() << "writeObjectPositionToBlackboard: invalid answer from blackboard: " << reply.get(0).toString();
            return false;
        }

        yInfo() << objectName+"Pose written to blackboard";

        return true;
    }

    bool writeObjectShapeToBlackboard(const std::string &objectName, Vector &shape)
    {
        if(shape.size() != 10)
        {
            yError() << "writeObjectShapeToBlackboard: invalid shape vector dimension";
            return false;
        }

        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "writeObjectShapeToBlackboard: no connection to blackboard";
            return false;
        }

        Bottle cmd;
        cmd.addString("set");
        cmd.addString(objectName+"Shape");
        Bottle &subcmd = cmd.addList();
        for(int i=0 ; i<10 ; i++)
        {
            subcmd.addDouble(shape[i]);
        }

        Bottle reply;
        blackboard_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "writeObjectShapeToBlackboard: invalid answer from blackboard: " << reply.toString();
            return false;
        }

        if(reply.get(0).asInt() != 1)
        {
            yError() << "writeObjectShapeToBlackboard: invalid answer from blackboard: " << reply.get(0).toString();
            return false;
        }

        yInfo() << objectName+"Shape written to blackboard";

        return true;
    }

    bool writeObjectDetectionStatusToBlackboard(const std::string &objectName, bool status)
    {
        if(blackboard_port.getOutputCount()<1)
        {
            yError() << "writeObjectDetectionStatusToBlackboard: no connection to blackboard";
            return false;
        }

        Bottle cmd;
        cmd.addString("set");
        cmd.addString(objectName+"Located");
        cmd.addString(status?"True":"False");

        Bottle reply;
        blackboard_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "writeObjectDetectionStatusToBlackboard: invalid answer from blackboard: " << reply.toString();
            return false;
        }

        if(reply.get(0).asInt() != 1)
        {
            yError() << "writeObjectDetectionStatusToBlackboard: invalid answer from blackboard: " << reply.get(0).toString();
            return false;
        }

        yInfo() << objectName+"Located set to" << (status?"True":"False") << "in blackboard";

        return true;
    }

    /****************************************************************/
    bool setGazeExploration(bool status)
    {
        if(gaze_exploration_port.getOutputCount()<1)
        {
            yError() << "setGazeExploration: no connection to gaze exploration module";
            return false;
        }

        Bottle cmd;
        cmd.addString("attention");
        cmd.addString(status?"start":"stop");

        Bottle reply;
        gaze_exploration_port.write(cmd, reply);

        if(reply.size() != 1)
        {
            yError() << "setGazeExploration: invalid answer from gaze exploration module: " << reply.toString();
            return false;
        }

        if(reply.get(0).asVocab() != Vocab::encode("ack"))
        {
            yError() << "setGazeExploration: invalid answer from gaze exploration module: " << reply.get(0).toString();
            return false;
        }

        return true;
    }

    /****************************************************************/
    ReturnStatus execute_tick(const std::string& params = "")
    {
        this->set_status(BT_RUNNING);

        Bottle paramsList;
        paramsList.fromString(params);

        std::string objectName("Bottle");
        double timeOut = 10;

        if(paramsList.size() > 0)
        {
            std::string object = paramsList.get(0).asString();
            if(object != "")
            {
                objectName = object;
            }
            else
            {
                yError()<<"execute_tick: invalid first parameter. Should be a string (object name).";
            }
        }

        if(paramsList.size() > 1)
        {
            double time = paramsList.get(1).asDouble();
            if(time > 0)
            {
                timeOut = time;
            }
            else
            {
                yError()<<"execute_tick: invalid second parameter. Should be a positive value (timeout).";
            }
        }

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        Vector position3D;
        bool objectLocated = false;

        // if object not already found run gaze exploration until the object is found
        if(!this->getObjectPosition(objectName, position3D))
        {
            if(!this->writeObjectDetectionStatusToBlackboard(objectName, true))
            {
                yError()<<"execute_tick: writeObjectPositionToBlackboard failed";
                this->set_status(BT_FAILURE);
                return BT_FAILURE;
            }

            // activate gaze exploration
            this->setGazeExploration(true);

            // wait until object is found or timeout

            double t0 = yarp::os::Time::now();
            while((!objectLocated) && (yarp::os::Time::now()-t0 < timeOut))
            {
                if(this->getHalted())
                {
                    this->set_status(BT_HALTED);
                    return BT_HALTED;
                }

                objectLocated = this->getObjectPosition(objectName, position3D);

                yarp::os::Time::delay(0.1);
            }

            this->setGazeExploration(false);

            if(!objectLocated)
            {
                yError()<<"execute_tick: object search timed out";
                this->set_status(BT_FAILURE);
                return BT_FAILURE;
            }
        }

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        Vector objectShape;
        if(!this->getObjectShape(objectName, objectShape))
        {
            yError()<<"execute_tick: getObjectShape failed";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        if(!this->writeObjectPositionToBlackboard(objectName, position3D))
        {
            yError()<<"execute_tick: writeObjectPositionToBlackboard failed";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(!this->writeObjectShapeToBlackboard(objectName, objectShape))
        {
            yError()<<"execute_tick: writeObjectShapeToBlackboard failed";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        if(!this->writeObjectDetectionStatusToBlackboard(objectName, true))
        {
            yError()<<"execute_tick: writeObjectPositionToBlackboard failed";
            this->set_status(BT_FAILURE);
            return BT_FAILURE;
        }

        this->set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("LocateBottle");

        this->configure_tick_server("/"+this->getName(), true);

        std::string objectPositionFetchPortName= "/"+this->getName()+"/objectPropertiesCollector/rpc:o";
        if (!object_properties_collector_port.open(objectPositionFetchPortName))
        {
           yError() << this->getName() << ": Unable to open port " << objectPositionFetchPortName;
           return false;
        }

        std::string gazeExplorationPortName= "/"+this->getName()+"/gazeExploration/rpc:o";
        if (!gaze_exploration_port.open(gazeExplorationPortName))
        {
           yError() << this->getName() << ": Unable to open port " << gazeExplorationPortName;
           return false;
        }

        std::string pointCloudReadPortName= "/"+this->getName()+"/pointCloudRead/rpc:o";
        if (!point_cloud_read_port.open(pointCloudReadPortName))
        {
           yError() << this->getName() << ": Unable to open port " << pointCloudReadPortName;
           return false;
        }

        std::string findSuperquadricPortName= "/"+this->getName()+"/findSuperquadric/rpc:o";
        if (!find_superquadric_port.open(findSuperquadricPortName))
        {
           yError() << this->getName() << ": Unable to open port " << findSuperquadricPortName;
           return false;
        }

        std::string blackboardPortName= "/"+this->getName()+"/blackboard/rpc:o";
        if (!blackboard_port.open(blackboardPortName))
        {
           yError() << this->getName() << ": Unable to open port " << blackboardPortName;
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
        object_properties_collector_port.interrupt();
        gaze_exploration_port.interrupt();
        point_cloud_read_port.interrupt();
        find_superquadric_port.interrupt();
        blackboard_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        object_properties_collector_port.close();
        gaze_exploration_port.close();
        point_cloud_read_port.close();
        find_superquadric_port.close();
        blackboard_port.close();

        return true;
    }
};


/****************************************************************/
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

    LocateBottle skill;

    return skill.runModule(rf);
}
