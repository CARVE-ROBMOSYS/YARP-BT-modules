/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <ReturnStatus.h>



int ReturnStatusVocab::fromString(const std::string& input) {
  // definitely needs optimizing :-)
  if (input=="BT_RUNNING") return (int)BT_RUNNING;
  if (input=="BT_SUCCESS") return (int)BT_SUCCESS;
  if (input=="BT_FAILURE") return (int)BT_FAILURE;
  if (input=="BT_IDLE") return (int)BT_IDLE;
  if (input=="BT_HALTED") return (int)BT_HALTED;
  if (input=="BT_ERROR") return (int)BT_ERROR;
  return -1;
}
std::string ReturnStatusVocab::toString(int input) const {
  switch((ReturnStatus)input) {
  case BT_RUNNING:
    return "BT_RUNNING";
  case BT_SUCCESS:
    return "BT_SUCCESS";
  case BT_FAILURE:
    return "BT_FAILURE";
  case BT_IDLE:
    return "BT_IDLE";
  case BT_HALTED:
    return "BT_HALTED";
  case BT_ERROR:
    return "BT_ERROR";
  }
  return "";
}


