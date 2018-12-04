
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
    is_halt_requested_ = false;
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
                    // waiting for execute_tick to start
                    std::this_thread::sleep_for( std::chrono::milliseconds(100) ); // TODO make a sleep that waits for a status that is not HALTED (i.e. the thread has started)
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


ReturnStatus TickServer::request_halt()
{
    ReturnStatus return_status = status_;
    switch (return_status)
    {
        case BT_ERROR:
            yError("The BT node %s returned error", module_name_.c_str());
            break;
        case BT_RUNNING:
            // nothing to do
        execute_halt();
            break;
        case BT_SUCCESS:
        case BT_FAILURE:
        case BT_IDLE:
        case BT_HALTED:
        // nothing to do
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


void TickServer::execute_halt()
{
    yTrace() << "Default dummy implementation. Make sure you don't need to re-implement this functions!";
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

ReturnStatus TickServer::request_status()
{
    return status_;
}


void TickServer::set_status(ReturnStatus status)
{
    status_ = status;
}

// Used by the skill to self-set error state
void TickServer::setErrorState(bool error)
{
    if(error)
        status_ = BT_ERROR;
    else
        status_ = BT_IDLE;
}




double TickServer::getPeriod()
{
    // module periodicity (seconds), called implicitly by the module.
    return 1.0;
}
// This is our main function. Will be called periodically every getPeriod() seconds
bool TickServer::updateModule()
{
    //cout << "[" << count << "]" << " updateModule..." << endl;
    return true;
}
// Message handler. Just echo all received messages.
bool TickServer::respond(const Bottle& command, Bottle& reply)
{

    return true;
}

bool TickServer::configure(yarp::os::ResourceFinder &rf)
{
    return true;
}

bool TickServer::interruptModule()
{
    return true;
}

// Close function, to perform cleanup.
bool TickServer::close()
{
    // optional, close port explicitly
    return true;
}

