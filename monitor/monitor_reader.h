/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file monitor_reader.h
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */


#ifndef MONITORREADER_H
#define MONITORREADER_H

#include <vector>
#include <string>
#include <yarp/os/RpcClient.h>

class MonitorReader
{
public:
    MonitorReader();
    bool configure(std::vector<std::__cxx11::string> &name_list);
    std::vector<std::string> updateList();

private :
    std::vector<yarp::os::Port*> rpc_ports_prt_list_;
};

#endif // MONITORREADER_H
