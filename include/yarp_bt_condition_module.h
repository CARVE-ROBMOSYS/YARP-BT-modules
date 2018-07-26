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
#pragma once
#include "yarp_bt_module.h"


class YARPBTConditionTickable : public YARPBTTickable
{
public:
    YARPBTConditionTickable(std::string name) : YARPBTTickable(name) {}
    virtual int tick() = 0;
    void halt() { }

};

