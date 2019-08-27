/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file robot_interaction.h
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */

#ifndef ROBOTINTERACTION_H
#define ROBOTINTERACTION_H

#include <vector>
#include <yarp/os/BufferedPort.h>
#include <yarp/BT_wrappers/blackboard_client.h>

class RobotInteraction
{
public:
    RobotInteraction();

    bool configure();
    bool sendMessage(std::vector<std::string> message_list);
    bool resetBlackboard();
    bool readMessage(std::string &message);
    bool closeDoor();
    bool openDoor();
private:
    yarp::os::BufferedPort<yarp::os::Bottle> robot_input_messages_port_;

    yarp::os::Port  world_interface_client_port_, robot_output_messages_port_;
    yarp::BT_wrappers::BlackBoardClient blackboardClient_;
};

#endif // ROBOTINTERACTION_H
