
/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file action_example_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */

//standard imports
#include <iostream>           // for std::cout
#include <chrono>             // for seconds
#include <thread>             //for this_thread::sleep_for
//YARP imports
#include <yarp/os/Network.h>  // for yarp::os::Network
#include <yarp/os/RFModule.h> // for yarp::os::ResourceFinder
#include <yarp/os/LogStream.h> // for yError()

//behavior trees imports
#include <yarp_bt_action_module.h>   // for YARPBTActionModule

class MyAction : public YARPBTActionModule
{
public:
    MyAction(std::string name) : YARPBTActionModule(name)
    {

    }
    ~MyAction(){}

    int Tick()
    {
        std::cout << "The Action is peforming some atomic operations" << std::endl;
        std::this_thread::sleep_for( std::chrono::seconds(5) );

        /*to make the action preemptable, it is preferable to add checks between atomic operations
         *to verify if the execution of the action is still required or not
         *you can verify this by calling the function is_halted()    */

        if(is_halt_requested())
        {
            std::cout << "The Action is no longer required. Execution stopped here" << std::endl;
            return BT_HALTED; //this return value is not really read. Just for consistency
        }

        std::cout << "The Action is peforming some other atomic operations" << std::endl;
        std::this_thread::sleep_for( std::chrono::seconds(5) );

        if(is_halt_requested())
        {
            std::cout << "The Action is no longer required. Execution stopped here" << std::endl;
            return BT_HALTED; //this return value is not really read. Just for consistency
        }

        std::cout << "The Action is peforming the last atomic operations" << std::endl;

        // after the action has performed its last operation, it defines if the action has succeded or not.
        bool action_has_succeded = true;
        std::this_thread::sleep_for( std::chrono::seconds(30) );

        if(is_halt_requested())
        {
            std::cout << "The Action is no longer required. Execution stopped here" << std::endl;
            return BT_HALTED; //this return value is not really read. Just for consistency
        }

        // return the status to the parent according to action_has_succeded
        if(action_has_succeded)
        {
            std::cout << "The Action has succeeded" << std::endl;
            return BT_SUCCESS;
        }
        else
        {
            std::cout << "The Action has failed" << std::endl;
            return BT_FAILURE;
        }
    }

    void Halt()
    {
        std::cout << "The Action is performing its Halt routine" << std::endl;
        std::this_thread::sleep_for( std::chrono::seconds(1) );

        /* The function Halt() is blocking. Hence you should put here the piece of code you want to execute
         * before continuing the execution of the Behavior Tree.
         * However, the code in Tick() between one is_halted() checkpoint and another is still being
         * executed in another thread. In some cases we do not want to waste time waiting for the code to reach
         * the next checkpoint (e.g. the action is writing in a port that nobody is reading) but is some other cases
         * we do need to wait for the code to reach the next checkpoint (e.g. the action is writing commands to a motor).
         * If we want to wait for the code to reach the next checkpoint, just add the following while loop.


        while (is_running())
        {
            std::cout << "The Action is still running, waiting for the code to reach the next checkpoint" << std::endl;
            std::this_thread::sleep_for( std::chrono::seconds(1) );

        }

        If not, do not add the while loop.
        */


        std::cout << "The Action has ended its Halt routine" << std::endl;

    }
};




int main(int argc, char * argv[])
{

    /* prepare and configure the resource finder */
    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    // print available command-line options
    // upon specifying --help; refer to xml
    // descriptor for the relative documentation
    if (rf.check("help"))
    {
        std::cout<<" This thrift has two services: request_tick() and request_halt() "<< std::endl;
        std::cout<<" request_tick() requests the execution of the action. returns 1 if it has succeded, 2 if it has failed "<< std::endl;
        std::cout<<" request_halt() requests the halt (abort) of the action "<< std::endl;
        std::cout<<" NOTE: the service request_tick() is blocking, hence if you need to halt the action, you need to"
                   " halt the action, you need to call request_halt() from another thread (e.g. another terminal) "<< std::endl;

        std::cout<< std::endl;
        return EXIT_SUCCESS;
    }

    /* initialize yarp network */
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available!";
        return EXIT_FAILURE;
    }


    std::cout << "Action ready. To send commands to the action, open and type: yarp rpc /ActionExample/cmd,"
              <<" then type help to find the available commands "
              << std::endl;

    /* create your module */
    MyAction module("ActionExample");

    rf.configure(argc, argv);
    module.runModule(rf);
    return 0;
}
