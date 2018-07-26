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
#ifndef YARPBTActionModule_H
#define YARPBTActionModule_H
#include "yarp_bt_module.h"


class YARPBTActionModule : public YARPBTModule
{
public:
    YARPBTActionModule(std::string name) : YARPBTModule(name)   {}
    virtual int tick() = 0;
    virtual void halt() = 0;

};


#endif // YARPBTActionModule_H
