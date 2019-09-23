
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
#include <yarp/os/PortablePair.h>
#include <yarp/BT_wrappers/MonitorMsg.h>

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
    _toMonitor_port.close();
}

bool TickClient::configure_TickClient(std::string portPrefix, std::string clientName)
{
    bool ret = true;
    if(clientName == "")
    {
        yError() << "Parameter <clientName> is mandatory";
    }

    _portPrefix = portPrefix;
    _clientName = clientName;

    // substitute blanks in name with underscore character
    std::replace(portPrefix.begin(), portPrefix.end(), ' ', '_');
    std::replace(clientName.begin(), clientName.end(), ' ', '_');

    std::string requestPort_name = portPrefix + "/" + clientName + "/tick:o";

    if (!_requestPort.open(requestPort_name.c_str())) {
        yError() << _clientName << ": Unable to open port " << requestPort_name;
        return false;
    }

    std::string monitorPort_name = portPrefix + "/" + clientName + "/monitor:o";
    ret = _toMonitor_port.open(monitorPort_name);

    if(!ret)
        _requestPort.close();
    else
        this->yarp().attachAsClient(_requestPort);

    return ret;
}

// This is useful to make sure all clients and servers have a matching suffix
bool TickClient::connect(std::string serverName)
{
    _serverName = serverName;
    return _requestPort.addOutput(serverName + "/tick:i");
}

ReturnStatus TickClient::request_tick(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params)
{
    yInfo() << "\tCalling tick on remote <" + _serverName + "> with target <" + target.target + "> and params <" + params.toString() + ">";

    // Propagate message to the monitor
    {   // additional scope, to cleanup the variables afterward
        yarp::BT_wrappers::MonitorMsg msg;
        msg.skill     = _serverName;
        msg.event     = "e_from_bt";
        _toMonitor_port.write(msg);
    }

    yInfo() << "\tCalling tick on remote <" + _serverName + "> with target <" + target.target + "> and params <" + params.toString() + ">";
    // Send the actual message to the server
    status_ = BT_request::request_tick(target, params);

    // Propagate message to the monitor
    {   // additional scope, to cleanup the variables afterward
        yarp::BT_wrappers::MonitorMsg msg;
        msg.skill     = _serverName;
        msg.event     = "e_to_bt";
        _toMonitor_port.write(msg);
    }
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

