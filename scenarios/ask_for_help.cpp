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
#include <iostream>             // for std::cout
#include <chrono>

//YARP imports
#include <yarp/os/Network.h>    // for yarp::os::Network
#include <yarp/os/LogStream.h>  // for yError()
#include <BTMonitorMsg.h>

//behavior trees imports
#include <include/tick_server.h>

using namespace yarp::os;

class AskForHelp : public TickServer, public RFModule
{

private:
    std::map<std::string, Value> bb;
    yarp::os::Port askHelp_port; // a bb port to handle messages
    yarp::os::Port toMonitor_port;

public:
    ReturnStatus request_tick(const std::string& params = "") override
    {
        ReturnStatus ret = BT_RUNNING;

        yDebug() << " Received tick";
        // send message to monitor: we are done with it
        BTMonitorMsg msg;
        msg.skill    = getName();
        msg.event    = "e_req";
        toMonitor_port.write(msg);
        set_status(ret);
        return ret;
    }

    ReturnStatus request_status()
    {
        ReturnStatus ret = BT_RUNNING;
        ReturnStatusVocab a;
        yInfo() << "request_status, replying with " << a.toString(ret);
        return ret;
    }

    ReturnStatus request_halt()
    {
        yDebug() << " Received halt";
        ReturnStatus ret = BT_HALTED;
        ReturnStatusVocab a;
        yInfo() << "request_halt,  replying with " << a.toString(ret);
        return ret;
    }

    double getPeriod()
    {
        // module periodicity (seconds), called implicitly by the module.
        return 1.0;
    }

    bool updateModule()
    {
        yInfo() << " Ask for help running happily...";
        return true;
    }

    // Message handler.
    bool respond(const Bottle& command, Bottle& reply)
    {
        if (command.get(0).asString() == "help given")
        {
            yInfo() << "Got 'help given' command";
            // send message to monitor: we are done with it
            BTMonitorMsg msg;
            msg.skill    = getName();
            msg.event    = "e_from_env";
            toMonitor_port.write(msg);
            yInfo() << "reply" << reply.toString();
        }

        return true;
    }

    bool configure(yarp::os::ResourceFinder &rf)
   {
       // optional, attach a port to the module
       // so that messages received from the port are redirected
       // to the respond method
       askHelp_port.open("/AskForHelp/rpc:i");
       attach(askHelp_port);

       configure_tick_server("/" + getName());
       toMonitor_port.open("/" + getName() + "/monitor:o");

       return true;
   }

    bool interruptModule()
    {
        askHelp_port.interrupt();
        toMonitor_port.interrupt();
        return true;
    }

    // Close function, to perform cleanup.
    bool close()
    {
        // optional, close port explicitly
        askHelp_port.close();
        toMonitor_port.close();
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
