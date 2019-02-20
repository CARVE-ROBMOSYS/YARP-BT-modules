/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_BTCmd
#define YARP_THRIFT_GENERATOR_BTCmd

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <ReturnStatus.h>

class BTCmd;


class BTCmd : public yarp::os::Wire {
public:
  BTCmd();
  virtual ReturnStatus request_tick(const std::string& params = "");
  virtual ReturnStatus request_status();
  virtual ReturnStatus request_halt();
  bool read(yarp::os::ConnectionReader& connection) override;
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
