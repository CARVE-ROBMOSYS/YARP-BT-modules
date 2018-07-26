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


class YARPBTActionTickable : public YARPBTTickable
{
public:
    YARPBTActionTickable(std::string name) : YARPBTTickable(name)   {}
    virtual int tick() = 0;
    virtual void halt() = 0;

};

