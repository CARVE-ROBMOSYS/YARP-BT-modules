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

#ifndef YARP_BT_CPP_COMMON_H
#define YARP_BT_CPP_COMMON_H

#include <yarp/BT_wrappers/ReturnStatus.h>
#include <behaviortree_cpp_v3//basic_types.h>

namespace bt_cpp_modules {

class iBT_CPP_modules {
  public:
    /**
     * @brief request_initialize  Hook for an initialization callback. The client can ask the
     *                            action server to perform an initialization step.
     * @return              true if initialization was successful, false otherwise
     */
    virtual bool initialize(yarp::os::Searchable &params) = 0;

    /**
     * @brief request_terminate  The client notifies the server to close, in order to perform a graceful shutdown.
     * @return              true if the call was successful, false otherwise
     */
    virtual bool terminate() = 0;
};

// Enum conversion function
static BT::NodeStatus toBT_cpp(yarp::BT_wrappers::ReturnStatus status)
{
    switch(status)
    {
        case yarp::BT_wrappers::ReturnStatus::BT_IDLE:
            return BT::NodeStatus::IDLE;

        case yarp::BT_wrappers::ReturnStatus::BT_RUNNING:
            return BT::NodeStatus::RUNNING;

        case yarp::BT_wrappers::ReturnStatus::BT_HALTED:  // BT_cpp library does not allow to return idle or halted!
        case yarp::BT_wrappers::ReturnStatus::BT_SUCCESS:
            return BT::NodeStatus::SUCCESS;

        case yarp::BT_wrappers::ReturnStatus::BT_ERROR:
        case yarp::BT_wrappers::ReturnStatus::BT_FAILURE:
            return BT::NodeStatus::FAILURE;

        default:
            return BT::NodeStatus::IDLE;
    }
}

static yarp::BT_wrappers::ReturnStatus toBT_cpp(BT::NodeStatus status)
{
    switch(status)
    {
        case BT::NodeStatus::IDLE:
            return yarp::BT_wrappers::ReturnStatus::BT_IDLE;

        case BT::NodeStatus::RUNNING:
            return yarp::BT_wrappers::ReturnStatus::BT_RUNNING;

        case BT::NodeStatus::SUCCESS:
        return yarp::BT_wrappers::ReturnStatus::BT_SUCCESS;

        case BT::NodeStatus::FAILURE:
            return yarp::BT_wrappers::ReturnStatus::BT_FAILURE;

        default:
            return yarp::BT_wrappers::ReturnStatus::BT_ERROR;
    }
}

}

#endif // YARP_BT_CPP_COMMON_H
