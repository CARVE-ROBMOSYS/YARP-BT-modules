
/******************************************************************************
*                                                                            *
* Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
* All Rights Reserved.                                                       *
*                                                                            *
******************************************************************************/
/**
 * @file yarp_bt_modules.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#include "btCpp_server.h"
#include "btCpp_common.h"

#include <yarp/os/LogStream.h>

using namespace BT;
using namespace std;
using namespace yarp::BT_wrappers;
using namespace bt_cpp_modules;

BtCpp_DummyServer::BtCpp_DummyServer(const std::string& name, const BT::NodeConfiguration& config) : ActionNodeBase(name, config)
{}

bool BtCpp_DummyServer::initialize(yarp::os::Searchable &params)
{
    YARP_UNUSED(params);

    string nodeName = string("/BT_engine/") +  this->name() + "/" + std::to_string(UID());

    // Make sure the required parameter is defined in the XML file
    Optional<std::string> targetName = getInput<std::string>("target");
    if(!targetName)
    {
        yError() << "Node" << this->name() << " failed to initialize: missing <target> parameter from XML file";
        return false;
    }

    yDebug() << "Initializing node " << nodeName;
    m_targetId.action_ID = (int32_t) UID();
    m_targetId.target = targetName.value();
    m_targetId.BT_filename = "";
    m_targetId.resources = {};

    m_params.clear();
    m_params.put("counter", yarp::os::Value(0));
    return m_blackBoardClient.setData(m_targetId.target, m_params);
}

bool BtCpp_DummyServer::terminate()
{
    return true;
}

NodeStatus BtCpp_DummyServer::tick()
{
    yInfo() << __FUNCTION__ << this->name();

    // get actual params from blackboard. This is done in the tick, so
    // parameters are always updated in case they changed in the blackboard
    m_params = m_blackBoardClient.getData(m_targetId.target);

    // Do stuff here ... simply increase the counter and update the blackboard.
    int currValue = m_params.find("counter").asInt32();
    m_params.put("counter", ++currValue);
    m_blackBoardClient.setData(m_targetId.target, m_params);
    return BT::NodeStatus::SUCCESS;
}


void BtCpp_DummyServer::halt()
{
    // Nothing to do here
    return;
}

