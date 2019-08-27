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

#ifndef YARP_BT_MODULES_BLACKBOARD_CLIENT_H
#define YARP_BT_MODULES_BLACKBOARD_CLIENT_H

#include <mutex>
#include <thread>
#include <atomic>

#include <yarp/os/Port.h>
#include <yarp/BT_wrappers/BT_request.h>
#include <yarp/BT_wrappers/BlackBoardWrapper.h>

// #include <BTMonitorMsg.h>   // TBD

namespace yarp {
namespace BT_wrappers {

class BlackBoardClient : private yarp::BT_wrappers::BlackBoardWrapper
{
public:
    BlackBoardClient();
    ~BlackBoardClient();

    /**
     * @brief configure     Open the yarp port to send tick requests.
     * @param portPrefix    Prefix for port name opened by the client. Must start with "/" character.
     * @param clientName    string identifying this client instance. Used primarly for monitoring.
     *                      Port opened by client will be <portPrefix> + "/" + <clientName> + "/tick:o"
     *                      <clientName> should not contain "/" character.
     * @return true if configuration was successful, false otherwise
     */
    bool configureBlackBoardClient(std::string portPrefix, std::string clientName);

    /**
     * @brief connect       Connect this tick client to the remote tick server
     * @param serverPort    name of the remote port to connect to. A 'tick:i' suffix
     *                      will be appended to <serverPort> param.
     * @return
     */
    bool connectToBlackBoard(const std::string serverPort="/blackboard");

    //Thrift services inherited from BTCmd
    /**
     * @brief Retrieve the Property associated to a key target
     * @param target name of the target to be retrieved
     * @return set of parameters associated
     *
     * yarp::os::Property getData(const std::string& target);
     */
    using BlackBoardWrapper::getData;

    /**
     * @brief Set addictional parameters to a key target
     * @param target name of the target to add parameters
     * @param datum set of parameters to be set.
     *
     * NOTE: the parameters will be merged on server side with the ones
     * already existings.
     *
     * bool setData(const std::string& target, const yarp::os::Property& datum);
     */
    using BlackBoardWrapper::setData;

    /**
     * @brief Clear all the content of the remote blackboard
     *
     * void clearAll();
     */
    using BlackBoardWrapper::clearAll;

    /**
     * @brief Clear all the data associated to a specified target
     * @param target name of the target to be cleared
     *
     * void clearData(const std::string& target);
     */

    using BlackBoardWrapper::clearData;

    /**
     * @brief Provide the list of all targets currently known to the blackboard
     * @return vector of targets
     *
     * std::vector<std::string> listTarget();
     */
    using BlackBoardWrapper::listTarget;

    /**
     * @brief Reset all blackboard memory to its initialization values from
     *        config file
     *
     * void resetData();
     */
    using BlackBoardWrapper::resetData;

    using BlackBoardWrapper::help;

private:
    std::string     m_portPrefix;
    std::string     m_clientName;
    yarp::os::Port  m_clientPort;
};

}}

#endif // YARP_BT_MODULES_BLACKBOARD_CLIENT_H
