
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

#include "btCpp_checkRobotInRoom.h"

#include <thread>
#include <iostream>
#include <yarp/os/LogStream.h>
#include <future>


using namespace std;
using namespace yarp::os;

using namespace BT;
using namespace yarp::BT_wrappers;
using namespace bt_cpp_modules;


BtCppCheckRootInRoom::BtCppCheckRootInRoom(const std::string& name, const BT::NodeConfiguration& config) :
      ConditionNode(name, config)
{  }

bool BtCppCheckRootInRoom::initialize(Searchable &params)
{
    YARP_UNUSED(params);

    m_clientName = this->name();
    std::replace(m_clientName.begin(), m_clientName.end(), ' ', '_');
    m_portPrefix = "/BT_engine/" + std::to_string(UID()) + "/" + m_clientName;

    // connect to YARP navigation server
    // TBD: In teory all the port names shall be here to be read as parameters,
    // but they will pollute the GUI. The right way shall be to use the
    // <Searchable params>.
    navClientConfig.put("device", "navigation2DClient");
    navClientConfig.put("local", m_portPrefix+"/navigationClient");
    navClientConfig.put("navigation_server", "/navigationServer");
    navClientConfig.put("map_locations_server", "/mapServer");
    navClientConfig.put("localization_server", "/localizationServer");
    bool ok = ddNavClient.open(navClientConfig);
    if (!ok)
    {
        yError() << "Unable to open navigation2DClient device driver";
        return false;
    }

    ok = ddNavClient.view(iNav);
    if (!ok)
    {
        yError() << "Unable to open INavigation2D interface";
        return false;
    }
    yInfo() << "Navigation client successfully initialized";


    // Get parameters from XML
    Optional<std::string> targetName = getInput<std::string>("target");
    if(!targetName)
    {
        yError() << "Node" << this->name() << " failed to initialize: missing <target> parameter from XML file";
        return false;
    }
    m_targetName = targetName.value();
    return true;
}


bool BtCppCheckRootInRoom::terminate()
{
    goTo_port.interrupt();
    pathPlanner_port.interrupt();
    goTo_port.close();
    pathPlanner_port.close();
    ddNavClient.close();
    return true;
}

BT::NodeStatus BtCppCheckRootInRoom::tick()
{
    BT::NodeStatus ret = NodeStatus::IDLE; // IDLE is a value not allowed in this engine. This will trigger an exception
    Optional<std::string> targetName = getInput<std::string>("target");
    if(!targetName || targetName.value() == "")
    {
        yError() << "Node" << this->name() << " can't get <target> for this action. Signalling failure by returning IDLE.";
        return BT::NodeStatus::IDLE;
    }

    m_targetName = targetName.value();

    yDebug() << "BtCppCheckRootInRoom::tick() " << this->name() << string(" checking target <") + m_targetName + ">";

    // TODO: do stuff here
    if(iNav->checkInsideArea(m_targetName))
    {
        ret = BT::NodeStatus::SUCCESS;
        yDebug() << "Robot reached target <" + m_targetName + "> location.";
    }
    else
        ret = BT::NodeStatus::FAILURE;

    yDebug() << "BtCppCheckRootInRoom::tick() " << this->name() << string(" with target <") + m_targetName + "> returns: " << toStr(ret);
    setStatus(ret);
    return ret;
}

