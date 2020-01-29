/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file yarp_bt_modules.h
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#ifndef YARP_BT_CPP_CHECK_ROBOT_AT_LOCATION_H
#define YARP_BT_CPP_CHECK_ROBOT_AT_LOCATION_H

#include "btCpp_common.h"
#include <yarp/BT_wrappers/tick_client.h>
#include <yarp/BT_wrappers/blackboard_client.h>

#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>

// Include lib from behaviortree_cpp
#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>


namespace bt_cpp_modules {

class BtCppCheckRobotAtLocation :   public BT::ConditionNode,        // inherit from BehaviorTree_cpp library
                                    public iBT_CPP_modules

{
public:
    BtCppCheckRobotAtLocation(const std::string& name, const BT::NodeConfiguration& config);

    /**
     * @brief configure     Open the yarp port to send tick requests.
     * @param portPrefix    Prefix for port name opened by the server. Must start with "/" character.
     * @param serverName    string identifying this server instance. Used primarly for monitoring.
     *                      Port opened by server will be <portPrefix> + "/" + <serverName> + "/tick:i"
     *                      <serverName> should not contain "/" character.
     * @return              true if configuration was successful, false otherwise
     */
    //
    bool initialize(yarp::os::Searchable &params) override;
    bool terminate() override;

    BT::NodeStatus tick() override;

    // It is mandatory to define this static method, from behavior tree CPP library
    static BT::PortsList providedPorts()
    {
        return { BT::InputPort("target",  "Name of the target location to check. Ex: <GraspLocation>"),
        };
    }
private:
    std::string         m_portPrefix;
    std::string         m_clientName;
    std::string         m_targetName;

    yarp::os::Property  navClientConfig;
    yarp::os::Port      goTo_port;
    yarp::os::Port      pathPlanner_port;

    yarp::dev::INavigation2D       *iNav = 0;
    yarp::dev::Nav2D::Map2DLocation       pos,curr_goal;
    yarp::dev::PolyDriver          ddNavClient;

    //Gets the current navigation status
    yarp::dev::NavigationStatusEnum status;
    double              linearTolerance{0.2}, angularTolerance{10};

    yarp::BT_wrappers::BlackBoardClient    m_blackboardClient;

};

}


#endif // YARP_BT_CPP_CHECK_ROBOT_AT_LOCATION_H
