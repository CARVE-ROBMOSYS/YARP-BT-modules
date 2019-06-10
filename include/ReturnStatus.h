/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_ENUM_ReturnStatus
#define YARP_THRIFT_GENERATOR_ENUM_ReturnStatus

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

enum ReturnStatus {
  BT_RUNNING = 0,
  BT_SUCCESS = 1,
  BT_FAILURE = 2,
  BT_IDLE = 3,
  BT_HALTED = 4,
  BT_ERROR = 5
};

class ReturnStatusVocab;

class ReturnStatusVocab : public yarp::os::idl::WireVocab {
public:
  int fromString(const std::string& input) override;
  std::string toString(int input) const override;
};


#endif
