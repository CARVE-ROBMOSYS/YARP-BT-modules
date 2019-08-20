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

#ifndef YARP_BT_CPP_SERVER_H
#define YARP_BT_CPP_SERVER_H

#include "btCpp_common.h"
#include <yarp/BT_wrappers/blackboard_client.h>


#include <mutex>
#include <atomic>
#include <thread>
#include <yarp/os/Port.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/LogStream.h>
#include <yarp/BT_wrappers/BT_request.h>
#include <experimental/propagate_const>

#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/behavior_tree.h>

namespace bt_cpp_modules {

class BtCpp_DummyServer :   public BT::ActionNodeBase,          // inherit from BehaviorTree_cpp library
                            public iBT_CPP_modules
{
public:
    BtCpp_DummyServer(const std::string& name, const BT::NodeConfiguration& config);

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
        return { BT::InputPort("target",    "Name of the target this action is refeered to, if any. "
                                            "In case addictional data are required, they will be fetched from blackboard using <target> as key."),
        };
    }

private:
    yarp::os::Property              m_params;
    yarp::BT_wrappers::ActionID            m_targetId;
    yarp::BT_wrappers::BlackBoardClient    m_blackBoardClient;
};

}

#endif // YARP_BT_CPP_SERVER_H
