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

#ifndef YARP_BT_MODULES_TICK_CLIENT_H
#define YARP_BT_MODULES_TICK_CLIENT_H

#include <mutex>
#include <thread>
#include <atomic>

#include <include/BTCmd.h>
#include <yarp/os/Port.h>

class TickClient : private BTCmd
{
public:
    TickClient();

    /**
     * @brief configure  Open the yarp port to send tick requests
     * @param name  name of the yarp port to open as a client. A 'tick:o' suffix
     * *            will be appended to 'aame' param
     * @return true if port was succesfully opened, false if error
     */
    bool configure(std::string name);

    /**
     * @brief connect   Connect tick client to remote tick server
     * @param serverName name of the remote port to connect to. A 'tick:i' suffix
     *                   will be appended to 'serverName' param
     * @return
     */
    bool connect(const std::string serverName);

    //Thrift services inherited from BTCmd
    ReturnStatus request_tick(const std::string &params = "");
    ReturnStatus request_status();
    ReturnStatus request_halt();

private:

    yarp::os::Port cmd_port_;
    std::string module_name_;

    std::atomic<ReturnStatus> status_;
    std::thread execute_tick_thread_;
};

#endif // YARP_BT_MODULES_TICK_CLIENT_H
