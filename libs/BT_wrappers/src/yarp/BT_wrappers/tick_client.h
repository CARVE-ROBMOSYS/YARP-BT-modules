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
     * @return true if configuration was successful, false otherwise
     */
    bool configure_TickClient(std::string portPrefix, std::string clientName);

    /**
     * @brief connect       Connect this tick client to the remote tick server.
     *                      NOTE: the connection for the monitoring port has to be done
     *                      externally, usually via yarpmanager
     * @param serverPort    name of the remote port to connect to. A 'tick:i' suffix
     *                      will be appended to <serverPort> param.
     * @return              true if success, false otherwise
     */
    bool connect(const std::string serverPort);

    //Thrift services inherited from BTCmd
    /**
     * @brief request_tick  Send a Tick request to the server, along with its parameters.
     *
     * @param target        The ActionID is a small structure useful to uniquely identify the client requesting the action,
     *                      so that the server can safely handle multiple clients at the same time.
     *                      Optional in case the action has no targets.
     * @param params        Any addictional parameter the server may require to perform the action.
     *                      Ex.: For navigation target may be the destination location name;
     *                      the params may contain the max speed.
     * @return              The enum indicating the status of the action on the server side.
     */
    ReturnStatus request_tick(  const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params = {}) override;

    /**
     * @brief request_halt  Send a Halt request to the server, along with its parameters.
     *
     * @param target        The ActionID is a small structure useful to uniquely identify the client requesting the action,
     *                      so that the server can safely handle multiple clients at the same time.
     *                      Optional in case the action has no targets.
     *                      NOTE: in case the Tick uses a target, then the Halt action MUST use the same target to stop it.
     * @param params        Any addictional parameter the server may require to perform the action.
     *
     *                      Ex.: For navigation target may be the destination location name;
     *                      the params may contain the max deceleration to apply.
     * @return              The enum indicating the status of the action on the server side.
     */
    ReturnStatus request_halt(  const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params = {}) override;

    /**
     * @brief request_status  Get the status of the action on the server side.
     *
     * @param target        The target whose state is required
     *                      NOTE: in case the Tick uses a target, then the request_status action MUST use the same target.
     *                      Ex.: For navigation target may be the destination location name.
     * @return              The enum indicating the status of the action on the server side.
     */
    ReturnStatus request_status(const yarp::BT_wrappers::ActionID &target) override;

    /**
     * @brief request_initialize  Hook for an initialization callback. The client can ask the action server to perform
     *                      an initialization step.
     * @return              true if initialization was successful, false otherwise
     */
    bool request_initialize() override;

    /**
     * @brief request_terminate  The client notifies the server to close, in order to perform a graceful shutdown.
     * @return              true if the call was successful, false otherwise
     */
    bool request_terminate()  override;

private:
    std::string _portPrefix;
    std::string _clientName;
    std::string _serverName;
    yarp::os::Port _requestPort;
    yarp::os::Port _toMonitor_port;

    std::atomic<yarp::BT_wrappers::ReturnStatus> status_;
};

}}

#endif // YARP_BT_MODULES_TICK_CLIENT_H
