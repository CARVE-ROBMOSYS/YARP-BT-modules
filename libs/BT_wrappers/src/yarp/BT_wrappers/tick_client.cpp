
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

#include "tick_client.h"

#include <memory>
#include <iostream>
#include <algorithm>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::BT_wrappers;

TickClient::TickClient() : BT_request()
{
    status_ = BT_IDLE;
}

TickClient::~TickClient()
{
    _requestPort.close();
    if(_monitorPort_p)
        _monitorPort_p->close();
}

bool TickClient::configure_TickClient(std::string portPrefix, std::string clientName, bool monitor)
{
    bool ret = true;
    if(clientName == "")
    {
        yError() << "Parameter <clientName> is mandatory";
    }

    _portPrefix = portPrefix;
    _clientName = clientName;

    std::string requestPort_name = portPrefix + "/" + clientName + "/tick:o";
    // substitute blanks in name with underscore character
    std::replace(requestPort_name.begin(), requestPort_name.end(), ' ', '_');
    if (!_requestPort.open(requestPort_name.c_str())) {
        yError() << _clientName << ": Unable to open port " << requestPort_name;
        return false;
    }

    if(monitor)
    {
        std::string monitorPort_name = portPrefix + "/" + clientName + "/monitor:o";
        useMonitor = true;
        _monitorPort_p = std::make_unique<yarp::os::Port>();
        ret = _monitorPort_p->open(monitorPort_name);
    }

    if(!ret)
    {
        _requestPort.close();
    }
    else
    {
        this->yarp().attachAsClient(_requestPort);
    }
    return ret;
}

// This is useful to make sure all clients and servers have a matching suffix
bool TickClient::connect(std::string serverName)
{
    _serverName = serverName;
    return _requestPort.addOutput(serverName + "/tick:i");
}

/*
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
        BTMonitorMsg msg;

        msg.skill    = module_name_;
        msg.event = "unknown";

        if((cmdType == BT_TICK) && (Direction::REQUEST == dir))
            msg.event = "e_from_bt"; 
        if((cmdType == BT_TICK) && (Direction::REPLY == dir))
            msg.event = "e_to_bt";

        if(cmdType == BT_STATUS)
            msg.event = "e_get_status";

        if((cmdType == BT_HALT) && (Direction::REQUEST == dir))
            msg.event = "halt_from_bt";
        if((cmdType == BT_HALT) && (Direction::REPLY == dir))
            msg.event = "halted_to_bt";

        toMonitor->write(msg);
    }
}
*/
ReturnStatus TickClient::request_tick(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params)
{
    // Propagate message to the monitor
//    yInfo() << " propagate command";
//    propagateCmd(BT_TICK, params);

    yInfo() << "\tCalling tick on target <" + target.target + "> with param <" + params.toString() + "> to remote server <" + _serverName + ">";
    // Send the actual message to the server
    status_ = BT_request::request_tick(target, params);

    ReturnStatusVocab a;
    yDebug() << "\treply is " << a.toString(status_);
    // Propagate reply to the monitor
//    propagateReply(BT_TICK, status_);

    return status_;
}

ReturnStatus TickClient::request_halt(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params)
{
    if(status_ == BT_RUNNING)
    {
        // Propagate message to the monitor
//        propagateCmd(BT_HALT);

        //I need halt the node
        return BT_request::request_halt(target, params);

        // Propagate reply to the monitor
//        propagateReply(BT_HALT, status_);
    }
}

ReturnStatus TickClient::request_status(const yarp::BT_wrappers::ActionID &target)
{
    return BT_request::request_status(target);
}

bool TickClient::request_initialize()
{
    return BT_request::request_initialize();
}

bool TickClient::request_terminate()
{
    return BT_request::request_terminate();
}

