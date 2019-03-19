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

#include <yarp/os/Port.h>
#include <BTCmd.h>
#include <TickCommand.h>
#include <Direction.h>
#include <BTMonitorMsg.h>


class TickClient : private BTCmd
{
public:
    TickClient();
    ~TickClient();

    /**
     * @brief configure  Open the yarp port to send tick requests
     * @param name  name of the yarp port to open as a client. A 'tick:o' suffix
     * *            will be appended to 'aame' param
     * @return true if port was succesfully opened, false if error
     */
    bool configure(std::string name,  bool monitor=false);

    /**
     * @brief connect   Connect tick client to remote tick server
     * @param serverName name of the remote port to connect to. A 'tick:i' suffix
     *                   will be appended to 'serverName' param
     * @return
     */
    bool connect(const std::string serverName);

    //Thrift services inherited from BTCmd
    ReturnStatus request_tick(const std::string &params = "") override;
    ReturnStatus request_status() override;
    ReturnStatus request_halt(const std::string &params = "") override;

private:
    std::string module_name_;
    std::string serverName_;
    yarp::os::Port cmd_port_;

    // monitor port is optional, so don't instantiate it if not required.
    bool useMonitor {false};
    std::unique_ptr<yarp::os::Port> toMonitor;

    std::atomic<ReturnStatus> status_;
    std::thread execute_tick_thread_;

    bool propagateCmd(TickCommand cmd, const std::string &params="");
    bool propagateReply(TickCommand cmd, ReturnStatus reply);
    bool propagateToMonitor(TickCommand cmdType, Direction dir, const std::string &params,  ReturnStatus reply=BT_IDLE);
};

#endif // YARP_BT_MODULES_TICK_CLIENT_H
