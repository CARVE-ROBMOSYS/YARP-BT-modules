
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

#include "btCpp_client.h"
#include "btCpp_common.h"

#include <memory>
#include <iostream>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;

using namespace BT;
using namespace yarp::BT_wrappers;
using namespace bt_cpp_modules;

BtCppClient::BtCppClient(const string &name, const BT::NodeConfiguration &config)  : ActionNodeBase(name, config)
{ }

bool BtCppClient::connect(std::string serverName)
{
    // connect to server
    bool ret = m_tickClient.connect(serverName);
    if(!ret)
    {
        yError() << "Failed to connect the module <" + this->name() + "> to <" + serverName + "> port.";
        return false;
    }
    else
        yInfo() << "Successfully connected the module <" + this->name() + "> to <" + serverName + "> port.";

    // connect to blackboard
    ret =  m_blackBoardClient.connectToBlackBoard();
    if(!ret)
    {
        yError() << "Failed to connect the module <" + this->name() + "> to the blackboard.";
        return false;
    }
    else
    {
        yInfo() << "Successfully connected the module <" + this->name() + "> to the blackboard.";
    }
    return true;
}

bool BtCppClient::initialize(Searchable &params)
{
    YARP_UNUSED(params);

    bool ret{true};
    string clientName = this->name() + "/" + std::to_string(UID());
    ret &= m_tickClient.configure_TickClient("/BT_engine", clientName);
    ret &= m_blackBoardClient.configureBlackBoardClient("/BT_engine", clientName);

    if(!ret)
    {
        yError() << this->name() << "initialization of client " << this->name() << " failed.";
        return false;
    }

    Optional<std::string> remote = getInput<std::string>("serverPort");
    // if we have a target, fetch the corresponding params from blackboard, if any
    if(remote)
    {
        m_serverPort = remote.value();
    }

    if(m_serverPort == "")
    {
        yError() << "serverPort parameter for node " << this->name() << " is missing";
        return false;
    }

    if(!connect(m_serverPort) )
    {
        yError() << "cannot connect to port <" + m_serverPort + ">";
        return false;
    }

    yWarning() << this->name() << "init middle";

    Optional<std::string> targetName = getInput<std::string>("target");
    if(targetName)
    {
        m_targetId.target = targetName.value();
    }
    else
    {
        yInfo() << "Node <" + this->name() + "> does not have a target";
        m_targetId.target = {};
    }

    // get resource list, if any
    Optional<std::string> resources = getInput<std::string>("resources");
    m_targetId.action_ID = (int32_t) UID();
    m_targetId.resources = resources.value();

    if(!m_tickClient.request_initialize())
    {
        yError() << "Node <" + this->name() + "> : Initialization on the remote server side failed";
        return false;
    }

    yDebug() << "Node <" + this->name() + "> : initialization done!";
    return true;
}

bool BtCppClient::terminate()
{

    return true;
}

NodeStatus BtCppClient::tick()
{
    yInfo() << "BtCppClient::tick() " << this->name();

    // get params from external blackboard.
    // This is inefficient... it'll be better to use internal blackboard in the future.

    Optional<std::string> targetName = getInput<std::string>("target");
    // if we have a target, fetch the corresponding params from blackboard, if any
    if(targetName)
    {
        m_params = m_blackBoardClient.getData(targetName.value());
        yInfo() << "Got data from blackboard " << m_params.toString();
    }

    yInfo() << "before tick() " << this->name();
    yarp::BT_wrappers::ReturnStatus ret = m_tickClient.request_tick(m_targetId, m_params);
    yInfo() << "after tick() " << this->name();
    return toBT_cpp(ret);
}


void BtCppClient::halt()
{
    yInfo() << "BtCppClient::halt() " << this->name();
    m_tickClient.request_halt(m_targetId, m_params);
    yInfo() << "BtCppClient::halt() DONE " << this->name();
    return;
}


