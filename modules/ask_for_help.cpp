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
#include <iostream>

//YARP imports
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
// #include <BTMonitorMsg.h>

using namespace yarp::os;
using namespace yarp::BT_wrappers;

class AskForHelp : public TickServer, public RFModule
{

private:
    std::map<std::string, Value> bb;
    yarp::os::Port receiveHelp_port; // a bb port to handle messages
    yarp::os::Port toMonitor_port;
    yarp::os::Port askHelp_port;

    bool helpReceived{false};

public:

    ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        ReturnStatus ret = BT_RUNNING;

        Bottle help;
        help.addString("Help requested");
        askHelp_port.write(help);

        yDebug() << " Received tick";
        // send message to monitor: we are done with it
/*
        BTMonitorMsg msg;
        msg.skill    = getName();
        msg.event    = "e_req";
        toMonitor_port.write(msg);
*/

        if(helpReceived)
        {
            ret = BT_SUCCESS;
/*
            BTMonitorMsg msg;
            msg.skill    = getName();
            msg.event    = "e_from_env";
            toMonitor_port.write(msg);
*/
        }
        return ret;
    }

    ReturnStatus request_halt(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        yDebug() << " Received halt ... nothing to do";
        return BT_HALTED;
    }

    double getPeriod()
    {
        // module periodicity (seconds), called implicitly by the module.
        return 1.0;
    }

    bool updateModule()
    {
        yInfo() << "Module " << this->getName() << " running happily...";
        return true;
    }

    // Message handler.
    bool respond(const Bottle& command, Bottle& reply)
    {
        if (command.get(0).asString() == "help given")
        {
            yInfo() << "Got 'help given' command";
            helpReceived  = true;

/*
            // send message to monitor: we are done with it
            BTMonitorMsg msg;
            msg.skill    = getName();
            msg.event    = "e_from_env";
            toMonitor_port.write(msg);
*/
        }

        return true;
    }

    bool configure(yarp::os::ResourceFinder &rf)
   {
       // optional, attach a port to the module
       // so that messages received from the port are redirected
       // to the respond method
       receiveHelp_port.open("/AskForHelp/rpc:i");
       askHelp_port.open("/AskForHelp/text:o");

       attach(receiveHelp_port);

       configure_TickServer("", getName(), false);
       toMonitor_port.open("/" + getName() + "/monitor:o");
       return true;
   }

    bool interruptModule()
    {
        receiveHelp_port.interrupt();
        toMonitor_port.interrupt();
        askHelp_port.interrupt();
        return true;
    }

    // Close function, to perform cleanup.
    bool close()
    {
        // optional, close port explicitly
        receiveHelp_port.close();
        toMonitor_port.close();
        askHelp_port.close();
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

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    AskForHelp module;
    module.setName("AskForHelp");
    module.runModule(rf);
    return 0;
}
