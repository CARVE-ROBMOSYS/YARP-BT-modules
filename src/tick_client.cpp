
/******************************************************************************
*                                                                            *
* Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
* All Rights Reserved.                                                       *
*                                                                            *
******************************************************************************/
/**
 * @file yarp_bt_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#include <iostream>
#include <thread>
#include <include/tick_client.h>

#include <yarp/os/LogStream.h>

TickClient::TickClient() : BTCmd()
{
    status_ = BT_IDLE;
}

bool TickClient::configure(std::string name)
{
    module_name_ = name;
    std::string cmd_port_name = module_name_ + "/tick:o";

    if (!cmd_port_.open(cmd_port_name.c_str())) {
        std::cout << module_name_ << ": Unable to open port " << cmd_port_name << std::endl;
        return false;
    }

    this->yarp().attachAsClient(cmd_port_);
    return true;
}

bool TickClient::connect(std::string serverName)
{
    std::cout << "Connecting to " << serverName + "/tick:i";
    return cmd_port_.addOutput(serverName + "/tick:i");
}

ReturnStatus TickClient::request_tick(const std::string &params)
{
//    yTrace() << "\n\t params " << params << " threaded " << threaded;
    status_ = BTCmd::request_tick(params);
    return status_;
}

ReturnStatus TickClient::request_halt()
{
    if(status_ == BT_RUNNING)
    {
        //I need halt the node
        BTCmd::request_halt();
    }
}

ReturnStatus TickClient::request_status()
{
    return BTCmd::request_status();
}

