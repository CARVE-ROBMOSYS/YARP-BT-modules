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

#ifndef YARP_bt_modulesS_TICK_SERVER_H
#define YARP_bt_modulesS_TICK_SERVER_H

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

#include <yarp/os/Port.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/LogStream.h>
#include <yarp/BT_wrappers/BT_request.h>

namespace yarp {
namespace BT_wrappers {

class TickServer :  public yarp::BT_wrappers::BT_request
{
public:
    TickServer();
    ~TickServer();

    /**
     * @brief configure     Open the yarp port to send tick requests.
     * @param portPrefix    Prefix for port name opened by the server. Must start with "/" character.
     * @param serverName    string identifying this server instance. Used primarly for monitoring.
     *                      Port opened by server will be <portPrefix> + "/" + <serverName> + "/tick:i"
     *                      <serverName> should not contain "/" character.
     * @param threaded      the execute_tick function will be called in a separated thread.
     *                      Use this option only if you know the tick function contains blocking calls.
     * @return              true if configuration was successful, false otherwise
     */
    //
    bool configure_TickServer(std::string portPrefix, std::string serverName, bool threaded=false);

    /**
     * @brief request_tick  Implement this method with user code to be run when the action receives the
     *                      Tick from Behaviour Tree.
     *                      The `request_tick` function must take a finite amounth of time before returning,
     *                      do not use long blocking calls.

     *                      In case of asynchronous action, i.e. the `request_tick` function takes some
     *                      to be executed and may contain blocking calls, then configure the server with
     *                      <threaded> parameter set to True.
     *                      That `request_tick` function will be executed in a separeted thread, and Tick
     *                      replies will be automatically sent back to the Behavour Tree engine without
     *                      interrupting the routine.
     *
     *                      On the receiving side, the server internally implements a bit
     *                      of logic to actually execute the tick only when the status is either idle
     *                      or halted, otherwise it will immediately return running.
     *
     *                      User implementation of `request_tick` therefore shall return only BT_SUCCESS,
     *                      BT_FAILURE or BT_ERROR and user can safely assume that this function will called
     *                      only for meaningful ticks.
     *
     * @param target        struct containing information about the action the Behaviour tree is ticking
     *                      and its target, if any.
     * @param params        parameters required by the server side implementation to correctly perform
     *                      the action requested by the Behaviour Tree. They are inputs for this server.
     *
     * @return              Enumerator indicating the current status of the action.
     */
    virtual ReturnStatus request_tick(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params = {}) override = 0;

    /**
     * @brief request_status    This function will return the current status of the action pointed by <target>.
     *                          It is useful to monitor the action behaviour or check whether or not an a
     *                          action is running
     * @param target            struct containing information about the action the Behaviour tree is ticking
     *                          and its target, if any.
     * @return                  Enumerator indicating the current status of the action.
     */
    virtual ReturnStatus request_status(const yarp::BT_wrappers::ActionID &target) override;

    /**
     * @brief request_halt      This function will halt the current action, if running
     * @param target            struct containing information about the action the Behaviour tree is ticking
     *                          and its target, if any.
     * @return                  Enumerator indicating the current status of the action after halt command has been
     *                          processed.
     */
    virtual ReturnStatus request_halt(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params = {}) override = 0 ;

    virtual bool request_initialize() override { return true; }

    virtual bool request_terminate()  override { return true; }

    /**
     * @brief isHaltRequested       Returns if a Halt has been requested on target
     * @return                      true if halt was requested, false otherwise
     */
    bool isHaltRequested(const yarp::BT_wrappers::ActionID target);

private:
    std::string     _portPrefix;
    std::string     _serverName;
    bool            _threaded {false};
    bool            _thread_finished{true};

    yarp::os::Port  _requestPort;

    std::mutex              _cv_mutex;
    std::condition_variable _cv_wait_for_thread;

    class RequestHandler;
    std::unique_ptr<RequestHandler> _requestHandler;
};

template <typename T>
class TickServer_withData :  public TickServer
{
private:
    std::map<ActionID, T> userData;

public:

    T& getData(ActionID id)
    {
        return userData[id];
    }

    void setData(ActionID id, T& data)
    {
        userData[id] = data;
    }

};

}}  // close namespaces

#endif // YARP_bt_modulesS_TICK_SERVER_H
