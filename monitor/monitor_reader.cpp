/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file monitor_reader.cpp
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */

#include "monitor_reader.h"
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcClient.h>


using namespace yarp::os;

MonitorReader::MonitorReader()
{

}

bool MonitorReader::configure(std::vector<std::string> name_list)
{

    // initializes yarp network
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available!";
        return false;
    }


    // opens the RPC ports used to request the states of the monitors
    for(std::vector<std::string>::iterator it = name_list.begin(); it != name_list.end(); ++it) {
        yInfo() << *it;

        yarp::os::Port* client_port = new  Port();
        std::string rpc_client_port_name = "/monitor/"+ *it + "/rpc:o";
        std::string rpc_server_port_name = "/monitor/"+ *it + "/rpc:i";

        client_port->open(rpc_client_port_name);
        bool is_connected = yarp.connect(rpc_client_port_name, rpc_server_port_name);

        if (!is_connected)
        {
            yError() << "[GUI] cannot connect to " << rpc_server_port_name;
            return false;
        }

        rpc_ports_prt_list_.push_back(client_port);
    }

    return true;
}


std::vector<std::string> MonitorReader::updateList()
{
    // requests the states of the monitors and collects them in a vector


    std::vector<std::string> return_list;

    for (int i = 0; i < rpc_ports_prt_list_.size(); i++)
    {
        yarp::os::Bottle cmd, reply;
        yarp::os::Port* client_port = rpc_ports_prt_list_.at(i);
        cmd.addString("state");
        client_port->write(cmd,reply);
        return_list.push_back(reply.toString());
    }

    return return_list;
}
