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
#ifndef YARPBTModule_H
#define YARPBTModule_H
#include "yarp/os/RFModule.h"
#include "yarp/os/Port.h"
#include "mutex"
#include <BTCmd.h>
class YARPBTModule : public BTCmd, public yarp::os::RFModule
{
public:
    YARPBTModule(std::string name);
    //inherited from RFModule
    bool attach(yarp::os::Port &source);
    bool configure( yarp::os::ResourceFinder &rf );
    bool updateModule();
    bool close();
    //Thrift services inherited from BehaviorTreeCmds
    int32_t request_tick();
    int32_t request_status();

    void request_halt();


    //Tick and Halt routine implemented by the user.
    virtual int Tick() = 0;
    virtual void Halt() = 0;




    //getters and setters for is_halted and is_running
    bool is_halt_requested();
    bool is_tick_requested();
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
     * - "BT_HALTED" indicates that the node has been halted by its father.
     * - "BT_EXIT" indicates the tree is going to be destroyied but is still wait for some nodes.*/
    enum ReturnStatus {BT_RUNNING, BT_SUCCESS, BT_FAILURE, BT_IDLE, BT_HALTED, BT_EXIT};
    int32_t get_status();
    void set_status(int32_t status);
    void halt_requested(bool is_halt_requested);
    void tick_requested(bool is_tick_requested);

private:
    yarp::os::Port cmd_port_;
    std::string module_name_;
    bool is_halted_;
    std::mutex is_halted_mutex_;
    bool is_halt_requested_;
    std::mutex is_halt_requested_mutex_;
    bool is_tick_requested_;
    std::mutex is_tick_requested_mutex_;
    bool is_running_;
    std::mutex is_running_mutex_;
    std::mutex status_mutex_;
    int32_t status_;
    bool is_paused_;
    double wait_for_;
};


class YARPBTAction : public YARPBTModule
{
public:
    YARPBTAction(std::string name) : YARPBTModule(name)   {}
    virtual int Tick() = 0;
    virtual void Halt() = 0;

};

class YARPBTCondition : public YARPBTModule
{
public:
    YARPBTCondition(std::string name) : YARPBTModule(name) {}
    virtual int Tick() = 0;
    void Halt() { }

};

#endif // YARPBTModule_H
