
/******************************************************************************
*                                                                            *
* Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)        *
* All Rights Reserved.                                                       *
*                                                                            *
******************************************************************************/
/**
 * @file yarp_bt_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#include "btCpp_setCondition.h"
#include "btCpp_common.h"

#include <memory>
#include <iostream>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;

using namespace BT;
using namespace bt_cpp_modules;
using namespace yarp::BT_wrappers;

BtCppSetCondition::BtCppSetCondition(const string &name, const BT::NodeConfiguration &config)  : ActionNodeBase(name, config)
{ }

bool BtCppSetCondition::configure_BtSetFlag(std::string portPrefix, std::string clientName, bool monitor)
{
    return m_blackBoardClient.configureBlackBoardClient(portPrefix, clientName);
}

bool BtCppSetCondition::connect(std::string serverName)
{
    return m_blackBoardClient.connectToBlackBoard();
}

bool BtCppSetCondition::initialize(Searchable &params)
{
    YARP_UNUSED(params);

    bool ret{false};
    ret  = configure_BtSetFlag("/BT_engine/", this->name() + "/" + std::to_string(UID()), false);

    Optional<std::string> remote = getInput<std::string>("serverPort");
    // if we have a target, fetch the corresponding params from blackboard, if any
    if(remote)
    {
        m_serverPort = remote.value();
    }

    if(m_serverPort == "")
    {
        yError() << "<serverPort> parameter for node " << this->name() << " is missing";
        return false;
    }
    ret &= connect(m_serverPort);

    if(!ret)
    {
        yError() << "Failed to connect to remote ports.";
        return false;
    }

    Optional<std::string> targetName = getInput<std::string>("target");
    if(!targetName)
    {
        yError() << "<target> parameter for node " << this->name() << " is missing";
        return false;
    }

    Optional<std::string> flagName = getInput<std::string>("flag");
    if(!flagName)
    {
        yError() << "<flag> parameter for node " << this->name() << " is missing";
        return false;
    }

    initValue(flagName.value());

    m_target = targetName.value();
    return true;
}

void BtCppSetCondition::initValue(string flagName)
{
    m_value.put(flagName, true);
    return;
}

bool BtCppSetCondition::terminate()
{
    return true;
}

//
//  SET FLAG to TRUE
//
NodeStatus BtCppSetCondition::tick()
{
    ReturnStatus ret;
    m_blackBoardClient.setData(m_target, m_value) ? ret = BT_SUCCESS : ret = BT_FAILURE;
    return toBT_cpp(ret);
}

void BtCppSetCondition::halt()
{
    // Nothing to do here
    return;
}

//
//  RESET FLAG to FALSE
//

BtCppResetCondition::BtCppResetCondition(const string &name, const BT::NodeConfiguration &config)  : BtCppSetCondition(name, config)
{ }

NodeStatus BtCppResetCondition::tick()
{
    ReturnStatus ret;
    m_blackBoardClient.setData(m_target, m_value) ? ret = BT_SUCCESS : ret = BT_FAILURE;
    return toBT_cpp(ret);
}

void BtCppResetCondition::initValue(string flagName)
{
    m_value.put(flagName, false);
    return;
}
