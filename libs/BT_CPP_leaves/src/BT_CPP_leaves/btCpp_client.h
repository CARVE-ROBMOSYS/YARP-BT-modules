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

#ifndef YARP_BT_CPP_CLIENT_H
#define YARP_BT_CPP_CLIENT_H

#include <mutex>
#include <thread>
#include <atomic>

#include "btCpp_common.h"

#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>

#include <yarp/BT_wrappers/tick_client.h>
#include <yarp/BT_wrappers/blackboard_client.h>
// #include <BTMonitorMsg.h>   // TBD

// Include lib from behaviortree_cpp
#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/behavior_tree.h>


namespace bt_cpp_modules {

class BtCppClient  : public BT::ActionNodeBase,          // inherit from BehaviorTree_cpp library
                     public iBT_CPP_modules
{
private:
    std::string   m_serverPort;
    yarp::os::Property                     m_params;
    yarp::BT_wrappers::ActionID            m_targetId;
    yarp::BT_wrappers::TickClient          m_tickClient;
    yarp::BT_wrappers::BlackBoardClient    m_blackBoardClient;

public:

    BtCppClient(const std::string& name, const BT::NodeConfiguration& config);

    bool initialize(yarp::os::Searchable &params) override;
    bool terminate() override;

    BT::NodeStatus tick() override;
    void halt() override;

    // It is mandatory to define this static method, from behavior tree CPP library
    static BT::PortsList providedPorts()
    {
        return { BT::InputPort("target",    "Name of the target this action is referring to, if any. "
                                            "In case addictional data are required, they will be fetched from blackboard using <target> as key."),
                 BT::InputPort("resources", "List of resources required by this action, if any."),
                 BT::InputPort("serverPort", "YARP Port Name to connect to.")
        };
    }
private:

    /**
     * @brief connect       Connect this tick client to the remote tick server
     * @param serverPort    name of the remote port to connect to. A 'tick:i' suffix
     *                      will be appended to <serverPort> param.
     * @return
     */
    bool connect(const std::string serverPort);
};


}

#endif // YARP_BT_CPP_CLIENT_H
