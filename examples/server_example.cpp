/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file condition_example_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

//standard imports
#include <chrono>
#include <iostream>             // for std::cout

//YARP imports
#include <yarp/os/Network.h>    // for yarp::os::Network
#include <yarp/os/LogStream.h>  // for yError()

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>

using namespace yarp::BT_wrappers;


class MySkillClass : public TickServer_withData<int>
{
public:
    ReturnStatus request_tick(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params) override
    {
        ReturnStatus ret = BT_ERROR;

        yDebug() << "Server example" << target.target << " with params " << params.toString();

        if(target.target == "Always_Success")
            ret = BT_SUCCESS;

        else if(target.target == "Always_Fail")
            ret = BT_FAILURE;

        if(target.target == "Delayed_Success")
        {
            std::this_thread::sleep_for( std::chrono::seconds(2));
            ret = BT_SUCCESS;
        }

        else if(target.target == "Delayed_Fail")
        {
            std::this_thread::sleep_for( std::chrono::seconds(2));
            ret = BT_FAILURE;
        }

        if(target.target == "Condition_Success")
            ret = BT_SUCCESS;

        else if(target.target == "Condition_Fail")
            ret = BT_FAILURE;

        else ret = BT_ERROR;

        ReturnStatusVocab a;
        std::this_thread::sleep_for( std::chrono::seconds(1));
        yInfo() << "Replying with " << a.toString((int)ret);
        return ret;
    }

    ReturnStatus request_status(const yarp::BT_wrappers::ActionID &target)
    {
        ReturnStatus ret = BT_RUNNING;
        ReturnStatusVocab a;
        yInfo() << "request_status, replying with " << a.toString(ret);
        return ret;
    }

    ReturnStatus request_halt(const yarp::BT_wrappers::ActionID &target, const yarp::os::Property &params = {}) override
    {
        ReturnStatus ret = BT_HALTED;
        ReturnStatusVocab a;
        yInfo() << "request_halt,  replying with " << a.toString(ret);
        return ret;
    }

    bool request_initialize() override
    {
        return true;
    }

    bool request_terminate() override
    {
        return true;
    }
};

int main(int argc, char * argv[])
{
    /* initialize yarp network */
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available!";
        return EXIT_FAILURE;
    }

    MySkillClass skill;
    skill.configure_TickServer("/TickServer", "Test_1", false);
/*
    std::cout << "Action ready. To send commands to the action, open and type: yarp rpc /ActionExample/cmd,"
              <<" then type help to find the available commands "
              << std::endl;
*/

    while (true)
    {
        std::this_thread::sleep_for( std::chrono::seconds(10));
        yInfo() << "Running";
    }
    return 0;
}
