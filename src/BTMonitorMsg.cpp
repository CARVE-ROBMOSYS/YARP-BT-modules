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

bool BTMonitorMsg::read_source(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(source)) {
    reader.fail();
    return false;
  }
  return true;
}
bool BTMonitorMsg::nested_read_source(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(source)) {
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
bool BTMonitorMsg::read_event(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(event)) {
    reader.fail();
    return false;
  }
  return true;
}
bool BTMonitorMsg::nested_read_event(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(event)) {
    reader.fail();
    return false;
  }
  return true;
}
bool BTMonitorMsg::read(yarp::os::idl::WireReader& reader) {
  if (!read_source(reader)) return false;
  if (!read_target(reader)) return false;
  if (!read_event(reader)) return false;
  return !reader.isError();
}

bool BTMonitorMsg::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(3)) return false;
  return read(reader);
}

bool BTMonitorMsg::write_source(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(source)) return false;
  return true;
}
bool BTMonitorMsg::nested_write_source(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(source)) return false;
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
bool BTMonitorMsg::write_event(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(event)) return false;
  return true;
}
bool BTMonitorMsg::nested_write_event(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(event)) return false;
  return true;
}
bool BTMonitorMsg::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_source(writer)) return false;
  if (!write_target(writer)) return false;
  if (!write_event(writer)) return false;
  return !writer.isError();
}

bool BTMonitorMsg::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  return write(writer);
}
bool BTMonitorMsg::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_source) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("source")) return false;
    if (!obj->nested_write_source(writer)) return false;
  }
  if (is_dirty_target) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("target")) return false;
    if (!obj->nested_write_target(writer)) return false;
  }
  if (is_dirty_event) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("event")) return false;
    if (!obj->nested_write_event(writer)) return false;
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
      if (field=="source") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string source")) return false;
      }
      if (field=="target") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string target")) return false;
      }
      if (field=="event") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string event")) return false;
      }
    }
    if (!writer.writeListHeader(4)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("source");
    writer.writeString("target");
    writer.writeString("event");
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
    if (key == "source") {
      will_set_source();
      if (!obj->nested_read_source(reader)) return false;
      did_set_source();
    } else if (key == "target") {
      will_set_target();
      if (!obj->nested_read_target(reader)) return false;
      did_set_target();
    } else if (key == "event") {
      will_set_event();
      if (!obj->nested_read_event(reader)) return false;
      did_set_event();
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
