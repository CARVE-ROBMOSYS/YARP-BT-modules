
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
#include <yarp/os/LogStream.h> // for yError()

#include <include/tick_client.h>

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
    client.configure("/tick_Y1");

/*
    std::cout << "Action ready. To send commands to the action, open and type: yarp rpc /ActionExample/cmd,"
              <<" then type help to find the available commands "
              << std::endl;
*/

    while (!client.connect("/tick_Y1"))
    {
        yInfo() << "Waiting for server to be ready";
        std::this_thread::sleep_for( std::chrono::seconds(1));
    }

    ReturnStatusVocab a;

    while (true)
    {
        int sleep = 5;
        yInfo() << "Running";
/*
        yInfo() << "request_status " << a.toString(client.request_status());
        std::this_thread::sleep_for( std::chrono::seconds(sleep));
*/
        std::cout << "request_tick cond_false "  << std::flush;
        std::cout << a.toString(client.request_tick("cond_false")) << std::endl;;
        std::this_thread::sleep_for( std::chrono::seconds(sleep));

        std::cout << "request_tick cond_true  ... " << std::flush;
        std::cout << a.toString(client.request_tick("cond_true")) << std::endl;
        std::this_thread::sleep_for( std::chrono::seconds(sleep));


        std::cout << "request_tick act_true   " << std::flush;
        std::cout << a.toString(client.request_tick("act_true")) << std::endl;;
        std::this_thread::sleep_for( std::chrono::seconds(sleep));

        std::cout << "request_tick act_false  " << std::flush;
        std::cout << a.toString(client.request_tick("act_false")) << std::endl;;
        std::this_thread::sleep_for( std::chrono::seconds(sleep));

        std::cout << "request_halt   "  << std::flush;
        std::cout << client.request_halt() << std::endl;;
        std::this_thread::sleep_for( std::chrono::seconds(sleep));
    }
    return 0;
}
