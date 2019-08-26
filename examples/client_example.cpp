
/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file action_example_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

//standard imports
#include <iostream>           // for std::cout
#include <chrono>             // for seconds
#include <thread>             //for this_thread::sleep_for

//YARP imports
#include <yarp/os/Network.h>  // for yarp::os::Network
#include <yarp/os/Property.h> // for yError()
#include <yarp/os/LogStream.h> // for yError()

#include <yarp/BT_wrappers/tick_client.h>

using namespace yarp::BT_wrappers;
using namespace yarp::os;

int main(int argc, char * argv[])
{
    /* initialize yarp network */
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available!";
        return EXIT_FAILURE;
    }

    TickClient client;
    client.configure_TickClient("/TickClient", "Test_1");

    while (!client.connect("/TickServer/Test_1"))
    {
        yInfo() << "Waiting for server to be ready";
        std::this_thread::sleep_for( std::chrono::seconds(1));
    }

    ReturnStatusVocab a;

    ActionID action;
    action.action_ID = 1;
    action.target    = "Always_Success";

    Property params;
    params.put("paramName", "paramValue");

    while (true)
    {
        int sleep = 2;
        yInfo() << "Running";

        std::this_thread::sleep_for( std::chrono::seconds(1));
        yDebug() << "request_tick " << action.target << " without params";
        yDebug() << a.toString(client.request_tick(action));
        std::this_thread::sleep_for( std::chrono::seconds(sleep));

        yDebug() << "request_tick " << action.target << " with params";
        yDebug() << a.toString(client.request_tick(action, params));
        std::this_thread::sleep_for( std::chrono::seconds(sleep));

        yDebug() << "request_tick " << action.target << " without params";
        yDebug() << a.toString(client.request_tick(action));
        std::this_thread::sleep_for( std::chrono::seconds(sleep));

        yDebug() << "request_tick " << action.target << " without params";
        yDebug() << a.toString(client.request_tick(action));
        std::this_thread::sleep_for( std::chrono::seconds(sleep));
    }
    return 0;
}
