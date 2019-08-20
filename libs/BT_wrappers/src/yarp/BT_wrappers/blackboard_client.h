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
    using BlackBoardWrapper::getData;
    using BlackBoardWrapper::setData;
    using BlackBoardWrapper::help;

private:
    std::string     m_portPrefix;
    std::string     m_clientName;
    yarp::os::Port  m_clientPort;
};

}}

#endif // YARP_BT_MODULES_BLACKBOARD_CLIENT_H
