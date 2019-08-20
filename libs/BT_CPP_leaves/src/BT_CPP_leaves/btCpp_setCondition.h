/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file bt_spp_set_flag.h
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#ifndef YARP_BT_CPP_SET_FLAG_H
#define YARP_BT_CPP_SET_FLAG_H

#include <mutex>
#include <thread>
#include <atomic>

#include "btCpp_common.h"

#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>

#include <yarp/BT_wrappers/tick_client.h>
#include <yarp/BT_wrappers/blackboard_client.h>
// #include <BTMonitorMsg.h>   // TBD

// Include lib from behaviortree_cpp repo
#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/behavior_tree.h>


namespace bt_cpp_modules {

class BtCppSetCondition : public BT::ActionNodeBase,          // inherit from BehaviorTree_cpp library
                          public iBT_CPP_modules
{
protected:
    std::string         m_target;
    std::string         m_serverPort;
    yarp::os::Property  m_value;

    yarp::BT_wrappers::BlackBoardClient    m_blackBoardClient;

    virtual void initValue(std::string flagName);

public:
    BtCppSetCondition(const std::string& name, const BT::NodeConfiguration& config);

    /**
     *      TODO: DOCUMENTATION HERE!!
     * @brief init
     * @param config
     * @param params
     * @return
     */

    bool initialize(yarp::os::Searchable &params) override;
    bool terminate() override;

    BT::NodeStatus tick() override;
    void halt() override;

    // It is mandatory to define this static method, from behavior tree CPP library
    static BT::PortsList providedPorts()
    {
        return { BT::InputPort("target",        "Name of the target the flag is refeered to."),
                 BT::InputPort("flag",          "Property of the target to be set to true."),
                 BT::InputPort("serverPort",    "YARP Port Name to connect to.")
        };
    }

private:

    /**
     * @brief configure     Open the yarp port to send tick requests.
     * @param portPrefix    Prefix for port name opened by the client. Must start with "/" character.
     * @param clientName    string identifying this client instance. Used primarly for monitoring.
     *                      Port opened by client will be <portPrefix> + "/" + <clientName> + "/tick:o"
     *                      <clientName> should not contain "/" character.
     * @param monitor       True activate monitoring messages, False monitoring messages will be disabled.
     *                      Portname for monitoring will be <portPrefix> + "/" + <clientName> + "/monitor:o"
     * @return true if configuration was successful, false otherwise
     */
    bool configure_BtSetFlag(std::string portPrefix, std::string clientName, bool monitor=false);

    /**
     * @brief connect       Connect this tick client to the remote tick server
     * @param serverPort    name of the remote port to connect to. A 'tick:i' suffix
     *                      will be appended to <serverPort> param.
     * @return
     */
    bool connect(const std::string serverPort);
};

class BtCppResetCondition : public BtCppSetCondition
{
  public:
    BtCppResetCondition(const std::string& name, const BT::NodeConfiguration& config);

    using BtCppSetCondition::initialize;
    using BtCppSetCondition::terminate;

    void initValue(std::string flagName) override;

    BT::NodeStatus tick() override;
    using BtCppSetCondition::halt;
};

}

#endif // YARP_BT_CPP_SET_FLAG_H
