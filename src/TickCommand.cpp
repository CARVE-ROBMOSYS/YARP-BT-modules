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
#include <TickCommand.h>



int TickCommandVocab::fromString(const std::string& input) {
  // definitely needs optimizing :-)
  if (input=="BT_TICK") return (int)BT_TICK;
  if (input=="BT_STATUS") return (int)BT_STATUS;
  if (input=="BT_HALT") return (int)BT_HALT;
  return -1;
}
std::string TickCommandVocab::toString(int input) const {
  switch((TickCommand)input) {
  case BT_TICK:
    return "BT_TICK";
  case BT_STATUS:
    return "BT_STATUS";
  case BT_HALT:
    return "BT_HALT";
  }
  return "";
}


