/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file condition_example_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */

//standard imports
#include <iostream>           // for std::cout

//YARP imports
#include <yarp/os/Network.h>  // for yarp::os::Network
#include <yarp/os/RFModule.h> // for yarp::os::ResourceFinder
#include <yarp/os/LogStream.h> // for yError()

//behavior trees imports
#include <yarp_bt_condition_module.h>   // for YARPBTConditionModule

class MyCondition : public YARPBTConditionModule
{
public:
    MyCondition(std::string name) : YARPBTConditionModule(name)
    {

    }
    ~MyCondition() {}
    int Tick()
    {
        std::cout << "The Condition is verifying a proposition" << std::endl;
        // return the status to the parent according to action_has_succeded

        bool is_proposition_true = true;
        if(is_proposition_true)
        {
            std::cout << "The Condition has succeeded" << std::endl;
            return BT_SUCCESS;
        }
        else
        {
            std::cout << "The Condition has failed" << std::endl;
            return BT_FAILURE;
        }
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
        std::cout<<" request_tick() requests the execution of the condition. returns 1 if the condition is "
                   " true, 2 if it is false "<< std::endl;

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

    std::cout << "Condition ready. To send commands to the action, open and type: yarp rpc /ConditionExample/cmd,"
              <<" then type help to find the available commands "
              << std::endl;


    /* create your module */
    MyCondition module("ConditionExample");

    module.runModule(rf);
    return 0;
}
