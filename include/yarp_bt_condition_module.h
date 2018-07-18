/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file yarp_bt_module.h
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */
#ifndef YARPBTConditionModule_H
#define YARPBTConditionModule_H
#include "yarp_bt_module.h"


class YARPBTConditionModule : public YARPBTModule
{
public:
    YARPBTConditionModule(std::string name) : YARPBTModule(name) {}
    virtual int Tick() = 0;
    void Halt() { }

};

#endif // YARPBTConditionModule_H
