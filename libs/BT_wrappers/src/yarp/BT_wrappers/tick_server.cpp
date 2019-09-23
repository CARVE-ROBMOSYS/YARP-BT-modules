
/******************************************************************************
*                                                                            *
* Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
* All Rights Reserved.                                                       *
*                                                                            *
******************************************************************************/
/**
 * @file yarp_bt_modules.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#include "tick_server.h"

#include <thread>
#include <future>
#include <iostream>
#include <algorithm>

#include <yarp/os/LogStream.h>
#include <yarp/os/PortablePair.h>
#include <yarp/BT_wrappers/MonitorMsg.h>

using namespace yarp::os;
using namespace yarp::BT_wrappers;

struct CompareActionID
{
    bool operator()(const ActionID& lhs, const ActionID& rhs) const {
        return lhs.action_ID < rhs.action_ID;
    }
};

class TickServer::RequestHandler: public BT_request
{
private:
    TickServer *_owner;

    // Helper struct for easy remapping of calls and data
    struct ActionData {
        ReturnStatus    status;
        std::atomic<bool> is_halt_requested;
        std::future<ReturnStatus> future_res;
        std::mutex              _cv_mutex;
        std::condition_variable _cv_wait_for_thread;

        ActionData() : status(BT_IDLE), is_halt_requested(false) {}
    };

    std::map<ActionID, ActionData, CompareActionID> _targetMap;

public:

    RequestHandler(TickServer * owner);

    // get Data associated to requested target; for TickServer internal use only
    ActionData &getData(ActionID target);

    bool request_initialize() override;

    bool request_terminate() override;

    ReturnStatus request_tick(const ActionID& target, const yarp::os::Property& params) override;

    ReturnStatus request_halt(const ActionID& target, const yarp::os::Property& params) override;

    ReturnStatus request_status(const ActionID& target) override;

private:
    void sendMonitorMessage_request();
    void sendMonitorMessage_reply();
    void sendMonitorMessage_reset();
};

TickServer::RequestHandler::RequestHandler(TickServer *owner) :  _owner(owner)
{

}

TickServer::RequestHandler::ActionData &TickServer::RequestHandler::getData(ActionID target)
{
    return _targetMap[target];
}

bool TickServer::RequestHandler:: request_initialize()
{
    return _owner->request_initialize();
}

bool TickServer::RequestHandler::request_terminate()
{
    return _owner->request_terminate();
}

void TickServer::RequestHandler::sendMonitorMessage_request()
{
    // Place here a message for monitoring: we received a tick msg
    yarp::BT_wrappers::MonitorMsg msg;
    msg.skill     = _owner->_serverName;
    msg.event     = "e_req";
    _owner->_toMonitor_port.write(msg);
}

void TickServer::RequestHandler::sendMonitorMessage_reply()
{
    // Place here a message for monitoring: we received a tick msg
    yarp::BT_wrappers::MonitorMsg msg;
    msg.skill     = _owner->_serverName;
    msg.event     = "e_from_env";
    _owner->_toMonitor_port.write(msg);
}

void TickServer::RequestHandler::sendMonitorMessage_reset()
{
    // Place here a message for monitoring: we received a tick msg
    yarp::BT_wrappers::MonitorMsg msg;
    msg.skill     = _owner->_serverName;
    msg.event     = "e_reset";
    _owner->_toMonitor_port.write(msg);
}

ReturnStatus TickServer::RequestHandler::request_tick(const ActionID& target, const yarp::os::Property& params)
{
    // Get ActionData corresponding to requested ActionID;
    // if ActionID is new, create the entry in the map
    ActionData &targetData =  _targetMap[target];
    ReturnStatus return_status = targetData.status;
    ReturnStatusVocab statusString;

    // for synch between tick and halt
    std::unique_lock<std::mutex> lk(targetData._cv_mutex);

    yDebug() << _owner->_serverName << " TickServer::RequestHandler::request_tick(target <" << target.target << \
                "> params <" << params.toString() << ">) threaded is " << _owner->_threaded << " status is " << statusString.toString(return_status);

    switch (return_status)
    {
        case BT_ERROR:
        {
            yError("TickServer::RequestHandler::request_tick: The BT node %s returned error", _owner->_serverName.c_str());
        } break;

        case BT_RUNNING:
        {
            /* nothing to do  -> return status will be updated by user callback,
             * so if status here is BT_RUNNING, it means the thread has still
             * job to be done.
             */
            std::future_status s = targetData.future_res.wait_for(std::chrono::steady_clock::duration::zero());
            if(s == std::future_status::ready)
            {
                targetData.status = targetData.future_res.get();
                if(targetData.status == BT_RUNNING)
                {
                    yError() << "request_tick shall not return BT_RUNNING.";
                    targetData.status == BT_ERROR;
                }
                return_status   = targetData.status;
            }
        } break;

        case BT_SUCCESS:
        case BT_FAILURE:
        {
            /* This tick the server will return Success / Failure to correctly acnowledge
             * the BT engine, without reactivating the user's routine.
             * Then the server state will be set to IDLE, so that next tick the routine
             * will start again as expected.
             */
            targetData.status = BT_IDLE;
        }
        break;

        case BT_IDLE:
        case BT_HALTED:
        {
            /* In case the routine is not running, then let's start it */
            if(_owner->_threaded)
            {
                // Place here a message for monitoring: we received a tick msg
                sendMonitorMessage_request();

                yDebug() << "Spawning thread";
                _owner->_thread_finished = false;
                targetData.future_res = std::async([this, &target, &params, &targetData]
                                                    {                    
                                                        ReturnStatus ret = _owner->request_tick(target, params);
                                                        // wake up condition variable
                                                        _owner->_thread_finished = true;
                                                        targetData._cv_wait_for_thread.notify_one();

                                                        // send message to monitor: we are done with it
                                                        sendMonitorMessage_reply();
                                                        return ret;
                                                    });
                targetData.status = BT_RUNNING;
                return_status   = targetData.status;
            }
            else
            {
                // Place here a message for monitoring: we received a meaningful tick msg
                sendMonitorMessage_request();

                /* In case user's routine is quick and does not require a separated thread,
                 * then the server will simply run it and returns the user's return value
                 */
                return_status = _owner->request_tick(target, params);

                // send message to monitor: we are done with it
                sendMonitorMessage_reply();
            }
        } break;

        default:
        {
            targetData.status = BT_ERROR;
            return_status   = targetData.status;
            yError("The BT node %s is in an unknown state.", _owner->_serverName.c_str());
            break;
        }
    }

    return return_status;
}

ReturnStatus TickServer::RequestHandler::request_halt(const ActionID& target, const yarp::os::Property& params)
{
    // Get ActionData corresponding to requested ActionID;
    // if ActionID is new, create the entry in the map
    ActionData &targetData =  _targetMap[target];
    ReturnStatus return_status = targetData.status;

    // TODO: check with Michele
    // Set is_halt_requested
    targetData.is_halt_requested = true;

    switch (return_status)
    {
        // Call the halt routine whenever it may be useful.
        case BT_ERROR:      // The user routine may handle the error, user have to check
        case BT_SUCCESS:
        case BT_FAILURE:
        {
            return_status = _owner->request_halt(target, params);
        } break;

        // In case the thread is running, then wait for it to finish.
        case BT_RUNNING:
        {
            return_status = _owner->request_halt(target, params);
            if(_owner->_threaded)
            {
                // wait until the running thread has finished
                std::unique_lock<std::mutex> cv_lock(targetData._cv_mutex);
                targetData._cv_wait_for_thread.wait(cv_lock, [this]{return this->_owner->_thread_finished;});
                cv_lock.unlock();
                yInfo() << "thread finished";
            }
        } break;

        case BT_IDLE:
        case BT_HALTED:
        {
            // nothing to do
        } break;

        default:
        {
            targetData.status = BT_ERROR;
            return_status     = targetData.status;
            yError("The BT node %s is in an unknown state (got value %d).", _owner->_serverName.c_str(), (int)return_status);
        } break;
    }

    // send message to the monitor
    sendMonitorMessage_reset();
    targetData.is_halt_requested = false;

    return return_status;
}

ReturnStatus TickServer::RequestHandler::request_status(const ActionID& target)
{
    return _targetMap[target].status;
}
//
// END of RequestHandler class
//


TickServer::TickServer() :  _requestHandler(std::make_unique<RequestHandler>(this))
{  }

TickServer::~TickServer()
{
    _requestPort.interrupt();
    _requestPort.close();

    _toMonitor_port.interrupt();
    _toMonitor_port.close();
}

bool TickServer::isHaltRequested(const yarp::BT_wrappers::ActionID target)
{
    return _requestHandler->getData(target).is_halt_requested;
}


bool TickServer::configure_TickServer(std::string portPrefix, std::string serverName, bool threaded)
{
    _portPrefix = portPrefix;
    _serverName = serverName;
    _threaded = threaded;

    if(serverName == "")
    {
        yError() << "Parameter <serverName> is mandatory";
    }

    std::replace(portPrefix.begin(), portPrefix.end(), ' ', '_');
    std::replace(serverName.begin(), serverName.end(), ' ', '_');

    std::string requestPort_name = portPrefix + "/" + serverName + "/tick:i";

    if (!_requestPort.open(requestPort_name.c_str())) {
        yError() << _serverName << ": Unable to open port " << requestPort_name;
        return false;
    }

    if(!_toMonitor_port.open(portPrefix + "/" + serverName +"/monitor:o") )
    {
        yError() << _serverName << ": Unable to open monitoring port " << (portPrefix + "/" + serverName +"/monitor:o");
        return false;
    }

    _requestHandler->yarp().attachAsServer(_requestPort);
    return true;
}

ReturnStatus TickServer::request_status(const yarp::BT_wrappers::ActionID &target)
{
    return _requestHandler->request_status(target);
}
