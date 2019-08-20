
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

#include "btCpp_checkCondition.h"

#include <thread>
#include <iostream>
#include <yarp/os/LogStream.h>
#include <future>


using namespace std;
using namespace yarp::os;

using namespace BT;
using namespace yarp::BT_wrappers;
using namespace bt_cpp_modules;


BtCppCheckCondition::BtCppCheckCondition(const std::string& name, const BT::NodeConfiguration& config) :
      ConditionNode(name, config)
{  }

bool BtCppCheckCondition::initialize(Searchable &params)
{
    YARP_UNUSED(params);
    m_portPrefix = "/BT_engine/" + std::to_string(UID());
    m_clientName = this->name();

    if(!m_tickClient.configure_TickClient(m_portPrefix, m_clientName, false) )
        return false;

    // Get parameters from XML
    Optional<std::string> remote = getInput<std::string>("serverPort");

    if(remote)
    {
        m_serverPort = remote.value();
    }

    if(m_serverPort == "")
    {
        m_serverPort = "/blackboard";
        yInfo() << "<serverPort> parameter for node <" + this->name() + "> is missing, using default <" + m_serverPort + ">.";
    }

    if(!m_tickClient.connect(m_serverPort) )
    {
        yError() << "Failed to connect the module <" + this->name() + "> to <" + m_serverPort + "> port.";
        return false;
    }
    else
        yInfo() << "Succesfully connected the module <" + this->name() + "> to <" + m_serverPort + "< port.";


    if(!m_tickClient.request_initialize())
    {
        yError() << "Node" << this->name() << ": Failed to initialize";
        return false;
    }

    Optional<std::string> targetName = getInput<std::string>("target");
    if(!targetName)
    {
        yError() << "Node" << this->name() << " failed to initialize: missing <target> parameter from XML file";
        return false;
    }

    Optional<std::string> flagName = getInput<std::string>("flag");
    if(!flagName)
    {
        yError() << "Node" << this->name() << " failed to initialize: missing <flag> parameter from XML file";
        return false;
    }

    m_targetId.action_ID = (int32_t) UID();
    m_targetId.target = targetName.value();
    m_targetId.BT_filename = "";
    m_targetId.resources = {};

    m_prop.put("flag", flagName.value());
    return true;
}


bool BtCppCheckCondition::terminate()
{
    // TODO: We shall disconnect and close ports here.
    return true;
}

BT::NodeStatus BtCppCheckCondition::tick()
{
    yInfo() << "BtCppCheckCondition::tick() " << this->name();
    BT::NodeStatus ret = toBT_cpp(m_tickClient.request_tick(m_targetId, m_prop));
    yInfo() << "BtCppCheckCondition::tick() " << this->name() << " ret: " << toStr(ret);
    setStatus(ret);
    return ret;
}

