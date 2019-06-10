/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_ENUM_TickCommand
#define YARP_THRIFT_GENERATOR_ENUM_TickCommand

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

/**
 * Enumerates the states every node can be in after execution during a particular
 * time step:
 * - "BT_SUCCESS" indicates that the node has completed running during this time step;
 * - "BT_FAILURE" indicates that the node has determined it will not be able to complete
 *   its task;
 * The following  returns statuses are not used in the thrift. But used in the Behavior
 * tree core. To make this project independend I just copied the enum from the core
 * (yes if someone changes the enum it could be dangerous).
 * The statuses are:
 * - "BT_RUNNING" indicates that the node has successfully moved forward during this
 *   time step, but the task is not yet complete;
 * - "BT_IDLE" indicates that the node hasn't run yet.
 * - "BT_HALTED" indicates that the node has been halted by its parent.
 * - "BT_ERROR" indicates the something wring happened.
 */
enum TickCommand {
  BT_TICK = 0,
  BT_STATUS = 1,
  BT_HALT = 2
};

class TickCommandVocab;

class TickCommandVocab : public yarp::os::idl::WireVocab {
public:
  int fromString(const std::string& input) override;
  std::string toString(int input) const override;
};


#endif
