
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

#include <iostream>
#include <thread>

#include <include/tick_client.h>

TickClient::TickClient() : BTCmd()
{
    module_name_ = "";
    set_halt_requested(false);
    set_status(BT_IDLE);
}

TickClient::TickClient(std::string name) : BTCmd()
{
    module_name_ = name;
    set_halt_requested(false);
    set_status(BT_IDLE);
}

bool TickClient::attach(yarp::os::Port &source)
{
    return this->yarp().attachAsServer(source);
}

bool TickClient::configure()
{
    configure(module_name_);
}

bool TickClient::configure(std::string name)
{
    module_name_ = name;
    std::string slash = "/";
    attach(cmd_port_);
    std::string cmd_port_name = "/";
    cmd_port_name += module_name_;
    cmd_port_name += "/cmd";
    if (!cmd_port_.open(cmd_port_name.c_str())) {
        std::cout << module_name_ << ": Unable to open port " << cmd_port_name << std::endl;
        return false;
    }
    return true;
}

int32_t TickClient::request_tick()
{
    int32_t return_status = get_status();


    switch (return_status) {
    case BT_ERROR:
        yError("The BT node %s returned error", module_name_.c_str());
        break;
    case BT_RUNNING:
        // nothing to do
        break;
    case BT_SUCCESS:
    case BT_FAILURE:
        set_status(BT_IDLE);
        break;
    case BT_IDLE:
    case BT_HALTED:
        execute_tick_thread_ = std::thread(&TickClient::executeTick, this);
        do
        {
            // waiting for executeTick to start
            std::this_thread::sleep_for( std::chrono::milliseconds(100) );
            return_status = get_status();
        }
        while (return_status == BT_IDLE);
        execute_tick_thread_.detach();
    default:
        break;
    }

    return return_status;

    if(return_status != BT_RUNNING)
    {
        //I need to execute the node
        execute_tick_thread_ = std::thread(&TickClient::executeTick, this);
        do
        {
            // waiting for executeTick to start
            std::this_thread::sleep_for( std::chrono::milliseconds(100) );
            return_status = get_status();
        }
        while (return_status == BT_IDLE);
    }
}

void TickClient::executeTick()
{
    set_status(BT_RUNNING);
    int32_t return_status = tick();
    set_status(return_status);

}

void TickClient::request_halt()
{
    set_halt_requested(true);
    int32_t return_status = get_status();
    if(return_status == BT_RUNNING)
    {
        //I need halt the node
        halt();
    }
}

ReturnStatus TickClient::request_status()
{
    return get_status();
}

ReturnStatus TickClient::get_status()
{
    std::lock_guard<std::mutex> lock(status_mutex_);
    return status_;

}

void TickClient::set_status(int32_t new_status)
{
    status_ = new_status;
}

bool TickClient::is_halt_requested()
{
    std::lock_guard<std::mutex> lock(is_halt_requested_mutex_);
    return is_halt_requested_;
}
void TickClient::set_halt_requested(bool is_halt_requested)
{
    std::lock_guard<std::mutex> lock(is_halt_requested_mutex_);
    is_halt_requested_ = is_halt_requested;
}


