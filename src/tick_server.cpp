
/******************************************************************************
*                                                                            *
* Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
* All Rights Reserved.                                                       *
*                                                                            *
******************************************************************************/
/**
 * @file yarp_bt_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#include <iostream>
#include <thread>

#include <include/tick_server.h>
#include <yarp/os/LogStream.h>

TickServer::TickServer()
{
    status_ = BT_IDLE;
}

bool TickServer::configure_tick_server(std::string name, bool threaded)
{
    module_name_ = name;
    threaded_ = threaded;

    std::string cmd_port_name = module_name_ + "/tick:i";
    if (!cmd_port_.open(cmd_port_name.c_str()))
    {
        std::cout << module_name_ << ": Unable to open port " << cmd_port_name << std::endl;
        return false;
    }
    this->yarp().attachAsServer(cmd_port_);
    return true;
}

ReturnStatus TickServer::request_tick(const std::string &params)
{
    yTrace() << "\n\t params " << params << " threaded " << threaded_;
    ReturnStatus return_status = status_;
    switch (return_status)
    {
        case BT_ERROR:
            yError("The BT node %s returned error", module_name_.c_str());
            break;
        case BT_RUNNING:
            // nothing to do
            break;
        case BT_SUCCESS:
        case BT_FAILURE:
            status_ = BT_IDLE;
            break;
        case BT_IDLE:
        case BT_HALTED:
            if(threaded_)
            {
                execute_tick_thread_ = std::thread(&TickServer::execute_tick, this, params);
                do
                {
                    // waiting for executeTick to start
                    std::this_thread::sleep_for( std::chrono::milliseconds(100) );
                    return_status = status_;
                }
                while (return_status == BT_IDLE);
                execute_tick_thread_.detach();
            }
            else
            {
                execute_tick(params);
                return_status = status_;
            }
            break;
        default:
            break;
    }

    return return_status;
}

ReturnStatus TickServer::execute_tick(const std::string& params)
{
    yTrace() << "Default dummy implementation. Please re-implement either this function or 'request_tick(...)' to do something meaningful";
    return BT_ERROR;
}


// Check if halt was requested by the BT engine
bool TickServer::getHalted()
{
    return is_halt_requested_;
}

// Used by the skill to self-set halt state  --> shall be allowed?
void TickServer::setHalted(bool halted)
{
    is_halt_requested_ = halted;
}

// Used by the skill to self-set error state
void TickServer::setErrorState(bool error)
{
    if(error)
        status_ = BT_ERROR;
    else
        status_ = BT_IDLE;
}