
/******************************************************************************
*                                                                            *
* Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
* All Rights Reserved.                                                       *
*                                                                            *
******************************************************************************/
/**
* @file yarp_bt_module.cpp
* @authors: Michele Colledanchise <michele.colledanchise@iit.it>
*/
#include "yarp_bt_module.h"

#include <iostream>
#include <thread>

YARPBTModule::YARPBTModule(std::string name) : BTCmd(), RFModule()
{
    module_name_ = name;
    halt_requested(false);
    tick_requested(false);
    status_ = BT_IDLE;
}
bool YARPBTModule::attach(yarp::os::Port &source)
{
    return this->yarp().attachAsServer(source);
}
bool YARPBTModule::configure(yarp::os::ResourceFinder &rf)
{
    std::string slash = "/";
    attach(cmd_port_);
    std::string cmd_port_name = "/";
    cmd_port_name += module_name_;
    cmd_port_name += "/cmd";
    if (!cmd_port_.open(cmd_port_name.c_str())) {
        std::cout << getName() << ": Unable to open port " << cmd_port_name << std::endl;
        return false;
    }
    return true;
}
bool YARPBTModule::updateModule()
{
    return true;
}
bool YARPBTModule::close()
{
    cmd_port_.close();
    return true;
}
int32_t YARPBTModule::request_tick()
{
    status_ = BT_RUNNING;
    int32_t return_status = Tick();
    status_ = return_status;
    if (is_halt_requested())
    {
        Halt();
        halt_requested(false);
        status_ = BT_HALTED;
        return BT_HALTED;
    }
    return return_status;
}
void YARPBTModule::request_halt()
{
    std::cout << " module received halt request " << std::endl;
    halt_requested(true);
}
int32_t YARPBTModule::request_status()
{
    return status_;
}
bool YARPBTModule::is_halt_requested()
{
    std::lock_guard<std::mutex> lock(is_halt_requested_mutex_);
    return is_halt_requested_;
}
void YARPBTModule::halt_requested(bool is_halt_requested)
{
    std::lock_guard<std::mutex> lock(is_halt_requested_mutex_);
    is_halt_requested_ = is_halt_requested;
}
bool YARPBTModule::is_tick_requested()
{
    std::lock_guard<std::mutex> lock(is_tick_requested_mutex_);
    return is_tick_requested_;
}
void YARPBTModule::tick_requested(bool is_tick_requested)
{
    std::lock_guard<std::mutex> lock(is_tick_requested_mutex_);
    is_tick_requested_ = is_tick_requested;
}
