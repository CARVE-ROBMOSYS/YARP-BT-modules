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

#ifndef YARP_BT_MODULES_TICK_CLIENT_H
#define YARP_BT_MODULES_TICK_CLIENT_H

#include <mutex>
#include <thread>
#include <atomic>

#include <yarp/os/Port.h>
#include <yarp/BT_wrappers/BT_request.h>

// #include <BTMonitorMsg.h>   // TBD

namespace yarp {
namespace BT_wrappers {

class TickClient : private yarp::BT_wrappers::BT_request
{
public:
    TickClient();
    ~TickClient();

    /**
     * @brief configure     Open the yarp port to send tick requests.
     * @param portPrefix    Prefix for port name opened by the client. Must start with "/" character.
     * @param clientName    string identifying this client instance. Used primarly for monitoring.
     *                      Port opened by client will be <portPrefix> + "/" + <clientName> + "/tick:o"
     *                      <clientName> should not contain "/" character.
     * @param monitor       True activate monitoring messages, False monitoring messages will be disabled.
     *                      Portname for monitoring will be <portPrefix> + "/" + <clientName> + "/monitor:o"
     * @return true if configuration was successful, false otherwise
     */
    bool configure_TickClient(std::string portPrefix, std::string clientName, bool monitor=false);

    /**
     * @brief connect       Connect this tick client to the remote tick server
     * @param serverPort    name of the remote port to connect to. A 'tick:i' suffix
     *                      will be appended to <serverPort> param.
     * @return
     */
    bool connect(const std::string serverPort);

    //Thrift services inherited from BTCmd
    ReturnStatus request_tick(  const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params = {}) override;
    ReturnStatus request_halt(  const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params = {}) override;
    ReturnStatus request_status(const yarp::BT_wrappers::ActionID &target) override;
    bool request_initialize() override;
    bool request_terminate()  override;

private:
    std::string _portPrefix;
    std::string _clientName;
    std::string _serverName;
    yarp::os::Port _requestPort;

    // monitor port is optional, so don't instantiate it if not required.
    bool useMonitor {false};
    std::unique_ptr<yarp::os::Port> _monitorPort_p;

    std::atomic<yarp::BT_wrappers::ReturnStatus> status_;
//    std::thread execute_tick_thread_;

/*
    bool propagateCmd(yarp::BT_wrappers::TickCommand cmd, const std::string &params="");
    bool propagateReply(yarp::BT_wrappers::TickCommand cmd, yarp::BT_wrappers::ReturnStatus reply);
    bool propagateToMonitor(yarp::BT_wrappers::TickCommand cmdType, Direction dir, const std::string &params,  yarp::BT_wrappers::ReturnStatus reply=yarp::BT_wrappers::BT_IDLE);
*/
};

}}

#endif // YARP_BT_MODULES_TICK_CLIENT_H
