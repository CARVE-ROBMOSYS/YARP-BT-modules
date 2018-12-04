/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file yarp_bt_module.h
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#ifndef YARP_BT_MODULES_TICK_SERVER_H
#define YARP_BT_MODULES_TICK_SERVER_H

#include "yarp/os/RFModule.h"
#include "yarp/os/Port.h"
#include "mutex"
#include <BTCmd.h>
#include <mutex>
#include <thread>
#include <atomic>

#include <yarp/os/LogStream.h>
#include <yarp/os/RFModule.h>

using namespace yarp::os;

class TickServer : public RFModule, public BTCmd
{
public:
    TickServer();

    bool attach_tick_server(yarp::os::Port &source);

    // threaded: when ticked, the execute_tick function will be called in a separated thread.
    //           Use this only if you know the tick function contains blocking calls.
    bool configure_tick_server(std::string name, bool threaded=false);

    // On the server side, the request_tick function implements a bit of logic to actually execute the tick
    // only when the status is either idle or halted, otherwise it will immediately return running.
    // Re-implement this function in case this helper logic is not required.
    virtual ReturnStatus request_tick(const std::string& params = "");


    // implementation of Thrift command request_status
    ReturnStatus request_status();

    void set_status(ReturnStatus status);
    ReturnStatus request_halt();

    // Request_tick will call this function when a meaningful tick is called, i.e. current status is
    // either idle or halted. If this implementation has blocking calls, set threaded to true in
    // the configuration.
    virtual ReturnStatus execute_tick(const std::string& params = "");

    // execute the halt of the node
    virtual void execute_halt();


    // Check if halt was requested by the BT engine
    bool getHalted();

    // Used by the skill to self-set halt state  --> shall be allowed?
    void setHalted(bool halted);

    // Used by the skill to self-set error state. If error is false, IDLE is set instead.
    void setErrorState(bool error);


    // implement classes of RFModule
    double getPeriod();
    // This is our main function. Will be called periodically every getPeriod() seconds
    bool updateModule();
    // Message handler. Just echo all received messages.
    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
    bool configure(yarp::os::ResourceFinder &rf);

    bool interruptModule();

    // Close function, to perform cleanup.
    bool close();





private:
    yarp::os::Port cmd_port_;
    std::string module_name_;

    bool threaded_ {false};
    std::thread execute_tick_thread_;
    std::atomic<ReturnStatus> status_;
    std::atomic<bool> is_halt_requested_;
};

#endif // YARP_BT_MODULES_TICK_SERVER_H
