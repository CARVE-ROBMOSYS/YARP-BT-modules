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
    RpcClient object_properties_collector_port;
    RpcClient blackboard_port;

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
            yError() << "getObjectPosition: invalid answer to object id query from objectPropertiesCollector module: empty id list:" << reply.toString();
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
            yError() << "getObjectPosition: invalid answer to object position query from objectPropertiesCollector module: object position not found: " << reply.toString();
            return false;
        }

        if(!reply.get(1).check("position_3d"))
        {
            yError() << "getObjectPosition: invalid answer to object position query from objectPropertiesCollector module: object position not found: " << reply.toString();
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

    /****************************************************************/
    ReturnStatus execute_tick(const std::string& objectName = "Bottle")
    {
        this->set_status(BT_RUNNING);

        if(this->getHalted())
        {
            this->set_status(BT_HALTED);
            return BT_HALTED;
        }

        Vector position3D;
        if(!this->getObjectPosition(objectName, position3D))
        {
            yError()<<"execute_tick: getObjectPosition failed";
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
        blackboard_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        object_properties_collector_port.close();
        blackboard_port.close();;

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
