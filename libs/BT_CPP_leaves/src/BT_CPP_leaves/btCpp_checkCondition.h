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

#ifndef YARP_BT_CPP_CHECK_CONDITION_H
#define YARP_BT_CPP_CHECK_CONDITION_H

#include "btCpp_common.h"
#include <yarp/BT_wrappers/tick_client.h>

#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>


// Include lib from behaviortree_cpp
#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/behavior_tree.h>


namespace bt_cpp_modules {

class BtCppCheckCondition : public BT::ConditionNode,        // inherit from BehaviorTree_cpp library
                            public iBT_CPP_modules

{
public:
    BtCppCheckCondition(const std::string& name, const BT::NodeConfiguration& config);

    bool initialize(yarp::os::Searchable &params) override;
    bool terminate() override;

    BT::NodeStatus tick() override;
//    void halt() override;

    // It is mandatory to define this static method, from behavior tree CPP library
    static BT::PortsList providedPorts()
    {
        return { BT::InputPort("target",        "Name of the target this action is refeered to. Ex: <bottle>"),
                 BT::InputPort("flag",          "Name of the flag to check, ex: <found>"),
                 BT::InputPort("serverPort",    "YARP Port Name to connect to.")
        };
    }
private:
    std::string             m_portPrefix;
    std::string             m_clientName;
    std::string             m_serverPort;
    yarp::BT_wrappers::TickClient  m_tickClient;

    // Store target and relative flag to check when ticked. This implementation
    // assumes they are defined in XML file and never change
    yarp::os::Property      m_prop;
    yarp::BT_wrappers::ActionID    m_targetId;
};

}


#endif // YARP_BT_CPP_CHECK_CONDITION_H
