/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <Direction.h>



int DirectionVocab::fromString(const std::string& input) {
  // definitely needs optimizing :-)
  if (input=="REQUEST") return (int)REQUEST;
  if (input=="REPLY") return (int)REPLY;
  return -1;
}
std::string DirectionVocab::toString(int input) const {
  switch((Direction)input) {
  case REQUEST:
    return "REQUEST";
  case REPLY:
    return "REPLY";
  }
  return "";
}


