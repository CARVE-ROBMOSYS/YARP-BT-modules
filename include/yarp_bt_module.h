/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file yarp_bt_module.h
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */
#pragma once

#include "yarp/os/RFModule.h"
#include "yarp/os/Port.h"
#include "mutex"
#include <BTCmd.h>
#include <mutex>
#include <thread>

class YARPBTTickable : public BTCmd//, public yarp::os::RFModule
{
public:
    YARPBTTickable(std::string name);
    bool attach(yarp::os::Port &source);
    bool configure();

    //Thrift services inherited from BTCmd
    int32_t request_tick();
    int32_t request_status();





    //Tick and Halt routine implemented by the user.
    virtual int tick() = 0;
    virtual void halt() = 0;



    /* Enumerates the states every node can be in after execution during a particular
     * time step:
     * - "BT_SUCCESS" indicates that the node has completed running during this time step;
     * - "BT_FAILURE" indicates that the node has determined it will not be able to complete
     *   its task;
     *
     * The following  returns statuses are not used in the thrift. But used in the Behavior
     * tree core. To make this project independend I just copied the enum from the core
     * (yes if someone changes the enum it could be dangerous). The statuses are:
     *
     *
     * - "BT_RUNNING" indicates that the node has successfully moved forward during this
     *   time step, but the task is not yet complete;
     * - "BT_IDLE" indicates that the node hasn't run yet.
     * - "BT_HALTED" indicates that the node has been halted by its parent.
     * - "BT_ERROR" indicates the something wring happened.*/

    enum ReturnStatus {BT_RUNNING, BT_SUCCESS, BT_FAILURE, BT_IDLE, BT_HALTED, BT_ERROR};

    // auxiliary functions
    void set_halt_requested(bool is_halt_requested);
    bool is_halt_requested(); // needed to let the node know that it cannot continue to execute code in the tick function
    void request_halt();
    void executeTick();

private:
    yarp::os::Port cmd_port_;
    std::string module_name_;

    bool is_halt_requested_;
    std::mutex is_halt_requested_mutex_;

    std::mutex status_mutex_;
    int32_t status_;


    int32_t get_status();
    void set_status(int32_t new_status);
    std::thread execute_tick_thread_;
};
