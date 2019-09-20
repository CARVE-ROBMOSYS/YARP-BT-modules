
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

#include "blackboard_client.h"

#include <memory>
#include <iostream>
#include <algorithm>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::BT_wrappers;

BlackBoardClient::BlackBoardClient() : BlackBoardWrapper()
{ }

BlackBoardClient::~BlackBoardClient()
{ }

bool BlackBoardClient::configureBlackBoardClient(std::string portPrefix, std::string clientName)
{
    if(clientName == "")
    {
        yError() << "Parameter <clientName> is mandatory";
    }

    m_portPrefix = portPrefix;
    m_clientName = clientName;

    std::string requestPort_name = portPrefix + "/" + clientName + "/blackboard/rpc:c";
    // substitute blanks in name with underscore character
    std::replace(requestPort_name.begin(), requestPort_name.end(), ' ', '_');
    if (!m_clientPort.open(requestPort_name.c_str())) {
        yError() << m_clientName << ": Unable to open port " << requestPort_name;
        return false;
    }

    return this->yarp().attachAsClient(m_clientPort);
}

bool BlackBoardClient::connectToBlackBoard(std::string serverName)
{
    std::string server{serverName + "/rpc:s"};
    yDebug() << "Connecting to " << server;
    return m_clientPort.addOutput(server);
}


