
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

#include <memory>
#include <iostream>

#include <yarp/os/LogStream.h>
#include <yarp/os/PortablePair.h>

#include <include/tick_client.h>

using namespace std;
using namespace yarp::os;

TickClient::TickClient() : BTCmd()
{
    status_ = BT_IDLE;
}

TickClient::~TickClient()
{
    cmd_port_.close();
    toMonitor->close();
}

bool TickClient::configure(std::string name)
{
    bool ret = true;
    module_name_ = name;
    std::string cmd_port_name = module_name_ + "/tick:o";

    if (!cmd_port_.open(cmd_port_name.c_str())) {
        std::cout << module_name_ << ": Unable to open port " << cmd_port_name << std::endl;
        return false;
    }

    if(useMonitor)
    {
        toMonitor = std::make_unique<yarp::os::Port>();
        ret = toMonitor->open(module_name_ + "/monitor:o");
    }

    this->yarp().attachAsClient(cmd_port_);
    return ret;
}

bool TickClient::connect(std::string serverName)
{
    std::cout << "Connecting to " << serverName + "/tick:i";
    serverName_ = serverName;
    return cmd_port_.addOutput(serverName_ + "/tick:i");
}

bool TickClient::propagateCmd(TickCommand cmd, const std::string &params)
{
    return propagateToMonitor(cmd, Direction::REQUEST, params);
}

bool TickClient::propagateReply(TickCommand cmd, ReturnStatus reply)
{
    return propagateToMonitor(cmd, Direction::REPLY, "", reply);
}

bool TickClient::propagateToMonitor(TickCommand cmdType, Direction dir, const string &params, ReturnStatus reply)
{
    if(useMonitor)
    {
        yarp::os::PortablePair<BTMonitorMsg, Bottle> monitor;
        BTMonitorMsg &msg = monitor.head;

        msg.requestor = module_name_;
        msg.target    = serverName_;
        msg.command   = cmdType;
        msg.direction = dir;
        msg.reply     = reply;
        monitor.body.addString(params);
        toMonitor->write(monitor);
    }
}

ReturnStatus TickClient::request_tick(const std::string &params)
{
//    yTrace() << "\n\t params " << params << " threaded " << threaded;

    // Propagate message to the monitor
    propagateCmd(BT_TICK, params);

    // Send the actual message to the server
    status_ = BTCmd::request_tick(params);

    // Propagate reply to the monitor
    propagateReply(BT_TICK, status_);

    return status_;
}

ReturnStatus TickClient::request_halt()
{
    if(status_ == BT_RUNNING)
    {
        // Propagate message to the monitor
        propagateCmd(BT_TICK);

        //I need halt the node
        BTCmd::request_halt();

        // Propagate reply to the monitor
        propagateReply(BT_TICK, status_);
    }
}

ReturnStatus TickClient::request_status()
{
    return BTCmd::request_status();
}

