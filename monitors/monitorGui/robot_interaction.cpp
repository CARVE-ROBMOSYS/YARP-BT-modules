/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file robot_interaction.cpp
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */


#include "robot_interaction.h"
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;

RobotInteraction::RobotInteraction()
{ }

bool RobotInteraction::configure()
{
    // initializes yarp network
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available! ";
        return false;
    }


    // opens the YARP port used to read messages from the robot

    bool is_open = robot_input_messages_port_.open("/GUI/robotMessages:i");

    if (!is_open)
    {
        yError() << "[GUI] cannot open port named /GUI/robotMessages:i";
        return false;
    }


    // opens the YARP port used to send messages from the robot

    is_open = robot_output_messages_port_.open("/GUI/robotMessages:o");

    if (!is_open)
    {
        yError() << "[GUI] cannot open port named /GUI/robotMessages:o";
        return false;
    }

    // opens the YARP blackboard client
    blackboardClient_.configureBlackBoardClient("/blackboard", "GUI");
    bool is_connected = blackboardClient_.connectToBlackBoard();

    if (!is_connected)
    {
        yError() << "[GUI] cannot connect to the blackboard.";
        return false;
    }

    // opens the YARP RPC port used to close the door in Gazebo simulator
    std::string world_client_port_name = "/GUI/world/rpc:o";
    std::string world_server_port_name = "/world_input_port";
    world_interface_client_port_.open(world_client_port_name);
    is_connected = yarp.connect(world_client_port_name, world_server_port_name);
    if (!is_connected)
    {
        yWarning() << "[GUI] cannot connect to " << world_server_port_name;
    }
    return true;
}


bool RobotInteraction::sendMessage(std::vector<std::string> message_list)
{
    Bottle message_btl;
    for (int i = 0; i< message_list.size(); i++)
    {
        message_btl.addString(message_list.at(i));
    }

    robot_output_messages_port_.write(message_btl);
    return true;
}

bool RobotInteraction::readMessage(std::string &message)
{
    Bottle* message_btl = robot_input_messages_port_.read(false);
    if (message_btl!=NULL)
    {
        message = message_btl->get(0).asString();
        return true;
    }
    return  false;
}

// wipe all data in the blackboard
bool RobotInteraction::resetBlackboard()
{
    blackboardClient_.resetData();
    return true;
}

bool RobotInteraction::closeDoor()
{
    Bottle cmd, reply;
    cmd.addString("setPose");
    cmd.addString("Door");
    cmd.addDouble(9.204);
    cmd.addDouble(0.221);
    cmd.addDouble(0.719);
    cmd.addDouble(0.0);
    cmd.addDouble(0.0);
    cmd.addDouble(1.330);

    world_interface_client_port_.write(cmd, reply);
    return true;
}


bool RobotInteraction::openDoor()
{
    Bottle cmd, reply;
    cmd.addString("setPose");
    cmd.addString("Door");
    cmd.addDouble(10.6);
    cmd.addDouble(0.0);
    cmd.addDouble(0.72);
    cmd.addDouble(0.0);
    cmd.addDouble(0.0);
    cmd.addDouble(1.37);

    world_interface_client_port_.write(cmd, reply);
    return true;
}
