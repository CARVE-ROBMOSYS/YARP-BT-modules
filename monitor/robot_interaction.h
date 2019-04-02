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
#include <yarp/os/BufferedPort.h>
#include <vector>
using namespace yarp::os;


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
    BufferedPort<Bottle> robot_input_messages_port_;
    yarp::os::Port blackboard_client_port_, world_interface_client_port_, robot_output_messages_port_;
};

#endif // ROBOTINTERACTION_H
