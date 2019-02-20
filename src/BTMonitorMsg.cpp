/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <BTMonitorMsg.h>

bool BTMonitorMsg::read_requestor(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(requestor)) {
    reader.fail();
    return false;
  }
  return true;
}
bool BTMonitorMsg::nested_read_requestor(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(requestor)) {
    reader.fail();
    return false;
  }
  return true;
}
bool BTMonitorMsg::read_target(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(target)) {
    reader.fail();
    return false;
  }
  return true;
}
bool BTMonitorMsg::nested_read_target(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(target)) {
    reader.fail();
    return false;
  }
  return true;
}
bool BTMonitorMsg::read_direction(yarp::os::idl::WireReader& reader) {
  int32_t ecast0;
  DirectionVocab cvrt1;
  if (!reader.readEnum(ecast0,cvrt1)) {
    reader.fail();
    return false;
  } else {
    direction = (Direction)ecast0;
  }
  return true;
}
bool BTMonitorMsg::nested_read_direction(yarp::os::idl::WireReader& reader) {
  int32_t ecast2;
  DirectionVocab cvrt3;
  if (!reader.readEnum(ecast2,cvrt3)) {
    reader.fail();
    return false;
  } else {
    direction = (Direction)ecast2;
  }
  return true;
}
bool BTMonitorMsg::read_command(yarp::os::idl::WireReader& reader) {
  int32_t ecast4;
   ::TickCommandVocab cvrt5;
  if (!reader.readEnum(ecast4,cvrt5)) {
    reader.fail();
    return false;
  } else {
    command = ( ::TickCommand)ecast4;
  }
  return true;
}
bool BTMonitorMsg::nested_read_command(yarp::os::idl::WireReader& reader) {
  int32_t ecast6;
   ::TickCommandVocab cvrt7;
  if (!reader.readEnum(ecast6,cvrt7)) {
    reader.fail();
    return false;
  } else {
    command = ( ::TickCommand)ecast6;
  }
  return true;
}
bool BTMonitorMsg::read_reply(yarp::os::idl::WireReader& reader) {
  int32_t ecast8;
   ::ReturnStatusVocab cvrt9;
  if (!reader.readEnum(ecast8,cvrt9)) {
    reader.fail();
    return false;
  } else {
    reply = ( ::ReturnStatus)ecast8;
  }
  return true;
}
bool BTMonitorMsg::nested_read_reply(yarp::os::idl::WireReader& reader) {
  int32_t ecast10;
   ::ReturnStatusVocab cvrt11;
  if (!reader.readEnum(ecast10,cvrt11)) {
    reader.fail();
    return false;
  } else {
    reply = ( ::ReturnStatus)ecast10;
  }
  return true;
}
bool BTMonitorMsg::read(yarp::os::idl::WireReader& reader) {
  if (!read_requestor(reader)) return false;
  if (!read_target(reader)) return false;
  if (!read_direction(reader)) return false;
  if (!read_command(reader)) return false;
  if (!read_reply(reader)) return false;
  return !reader.isError();
}

bool BTMonitorMsg::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(5)) return false;
  return read(reader);
}

bool BTMonitorMsg::write_requestor(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(requestor)) return false;
  return true;
}
bool BTMonitorMsg::nested_write_requestor(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(requestor)) return false;
  return true;
}
bool BTMonitorMsg::write_target(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(target)) return false;
  return true;
}
bool BTMonitorMsg::nested_write_target(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(target)) return false;
  return true;
}
bool BTMonitorMsg::write_direction(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)direction)) return false;
  return true;
}
bool BTMonitorMsg::nested_write_direction(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)direction)) return false;
  return true;
}
bool BTMonitorMsg::write_command(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)command)) return false;
  return true;
}
bool BTMonitorMsg::nested_write_command(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)command)) return false;
  return true;
}
bool BTMonitorMsg::write_reply(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)reply)) return false;
  return true;
}
bool BTMonitorMsg::nested_write_reply(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)reply)) return false;
  return true;
}
bool BTMonitorMsg::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_requestor(writer)) return false;
  if (!write_target(writer)) return false;
  if (!write_direction(writer)) return false;
  if (!write_command(writer)) return false;
  if (!write_reply(writer)) return false;
  return !writer.isError();
}

bool BTMonitorMsg::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(5)) return false;
  return write(writer);
}
bool BTMonitorMsg::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_requestor) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("requestor")) return false;
    if (!obj->nested_write_requestor(writer)) return false;
  }
  if (is_dirty_target) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("target")) return false;
    if (!obj->nested_write_target(writer)) return false;
  }
  if (is_dirty_direction) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("direction")) return false;
    if (!obj->nested_write_direction(writer)) return false;
  }
  if (is_dirty_command) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("command")) return false;
    if (!obj->nested_write_command(writer)) return false;
  }
  if (is_dirty_reply) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("reply")) return false;
    if (!obj->nested_write_reply(writer)) return false;
  }
  return !writer.isError();
}
bool BTMonitorMsg::Editor::read(yarp::os::ConnectionReader& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) return false;
  int len = reader.getLength();
  if (len==0) {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(1)) return false;
    writer.writeString("send: 'help' or 'patch (param1 val1) (param2 val2)'");
    return true;
  }
  std::string tag;
  if (!reader.readString(tag)) return false;
  if (tag=="help") {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("many",1, 0)) return false;
    if (reader.getLength()>0) {
      std::string field;
      if (!reader.readString(field)) return false;
      if (field=="requestor") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string requestor")) return false;
      }
      if (field=="target") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string target")) return false;
      }
      if (field=="direction") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("Direction direction")) return false;
      }
      if (field=="command") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(" ::TickCommand command")) return false;
      }
      if (field=="reply") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(" ::ReturnStatus reply")) return false;
      }
    }
    if (!writer.writeListHeader(6)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("requestor");
    writer.writeString("target");
    writer.writeString("direction");
    writer.writeString("command");
    writer.writeString("reply");
    return true;
  }
  bool nested = true;
  bool have_act = false;
  if (tag!="patch") {
    if ((len-1)%2 != 0) return false;
    len = 1 + ((len-1)/2);
    nested = false;
    have_act = true;
  }
  for (int i=1; i<len; i++) {
    if (nested && !reader.readListHeader(3)) return false;
    std::string act;
    std::string key;
    if (have_act) {
      act = tag;
    } else {
      if (!reader.readString(act)) return false;
    }
    if (!reader.readString(key)) return false;
    // inefficient code follows, bug paulfitz to improve it
    if (key == "requestor") {
      will_set_requestor();
      if (!obj->nested_read_requestor(reader)) return false;
      did_set_requestor();
    } else if (key == "target") {
      will_set_target();
      if (!obj->nested_read_target(reader)) return false;
      did_set_target();
    } else if (key == "direction") {
      will_set_direction();
      if (!obj->nested_read_direction(reader)) return false;
      did_set_direction();
    } else if (key == "command") {
      will_set_command();
      if (!obj->nested_read_command(reader)) return false;
      did_set_command();
    } else if (key == "reply") {
      will_set_reply();
      if (!obj->nested_read_reply(reader)) return false;
      did_set_reply();
    } else {
      // would be useful to have a fallback here
    }
  }
  reader.accept();
  yarp::os::idl::WireWriter writer(reader);
  if (writer.isNull()) return true;
  writer.writeListHeader(1);
  writer.writeVocab(yarp::os::createVocab('o','k'));
  return true;
}

std::string BTMonitorMsg::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}
