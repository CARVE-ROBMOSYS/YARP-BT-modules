/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_BTMonitorMsg
#define YARP_THRIFT_GENERATOR_STRUCT_BTMonitorMsg

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <Direction.h>
#include <ReturnStatus.h>
#include <TickCommand.h>

class BTMonitorMsg;


class BTMonitorMsg : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::string requestor;
  std::string target;
  Direction direction;
   ::TickCommand command;
   ::ReturnStatus reply;

  // Default constructor
  BTMonitorMsg() : requestor(""), target(""), direction((Direction)0), command(( ::TickCommand)0), reply(( ::ReturnStatus)0) {
  }

  // Constructor with field values
  BTMonitorMsg(const std::string& requestor,const std::string& target,const Direction direction,const  ::TickCommand command,const  ::ReturnStatus reply) : requestor(requestor), target(target), direction(direction), command(command), reply(reply) {
  }

  // Copy constructor
  BTMonitorMsg(const BTMonitorMsg& __alt) : WirePortable(__alt)  {
    this->requestor = __alt.requestor;
    this->target = __alt.target;
    this->direction = __alt.direction;
    this->command = __alt.command;
    this->reply = __alt.reply;
  }

  // Assignment operator
  const BTMonitorMsg& operator = (const BTMonitorMsg& __alt) {
    this->requestor = __alt.requestor;
    this->target = __alt.target;
    this->direction = __alt.direction;
    this->command = __alt.command;
    this->reply = __alt.reply;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_requestor(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_requestor(const yarp::os::idl::WireWriter& writer) const;
  bool write_target(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_target(const yarp::os::idl::WireWriter& writer) const;
  bool write_direction(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_direction(const yarp::os::idl::WireWriter& writer) const;
  bool write_command(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_command(const yarp::os::idl::WireWriter& writer) const;
  bool write_reply(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_reply(const yarp::os::idl::WireWriter& writer) const;
  bool read_requestor(yarp::os::idl::WireReader& reader);
  bool nested_read_requestor(yarp::os::idl::WireReader& reader);
  bool read_target(yarp::os::idl::WireReader& reader);
  bool nested_read_target(yarp::os::idl::WireReader& reader);
  bool read_direction(yarp::os::idl::WireReader& reader);
  bool nested_read_direction(yarp::os::idl::WireReader& reader);
  bool read_command(yarp::os::idl::WireReader& reader);
  bool nested_read_command(yarp::os::idl::WireReader& reader);
  bool read_reply(yarp::os::idl::WireReader& reader);
  bool nested_read_reply(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<BTMonitorMsg > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new BTMonitorMsg;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(BTMonitorMsg& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(BTMonitorMsg& obj, bool dirty = true) {
      if (obj_owned) delete this->obj;
      this->obj = &obj;
      obj_owned = false;
      dirty_flags(dirty);
      return true;
    }

    virtual ~Editor() {
    if (obj_owned) delete obj;
    }

    bool isValid() const {
      return obj!=0/*NULL*/;
    }

    BTMonitorMsg& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_requestor(const std::string& requestor) {
      will_set_requestor();
      obj->requestor = requestor;
      mark_dirty_requestor();
      communicate();
      did_set_requestor();
    }
    void set_target(const std::string& target) {
      will_set_target();
      obj->target = target;
      mark_dirty_target();
      communicate();
      did_set_target();
    }
    void set_direction(const Direction direction) {
      will_set_direction();
      obj->direction = direction;
      mark_dirty_direction();
      communicate();
      did_set_direction();
    }
    void set_command(const  ::TickCommand command) {
      will_set_command();
      obj->command = command;
      mark_dirty_command();
      communicate();
      did_set_command();
    }
    void set_reply(const  ::ReturnStatus reply) {
      will_set_reply();
      obj->reply = reply;
      mark_dirty_reply();
      communicate();
      did_set_reply();
    }
    const std::string& get_requestor() {
      return obj->requestor;
    }
    const std::string& get_target() {
      return obj->target;
    }
    const Direction get_direction() {
      return obj->direction;
    }
    const  ::TickCommand get_command() {
      return obj->command;
    }
    const  ::ReturnStatus get_reply() {
      return obj->reply;
    }
    virtual bool will_set_requestor() { return true; }
    virtual bool will_set_target() { return true; }
    virtual bool will_set_direction() { return true; }
    virtual bool will_set_command() { return true; }
    virtual bool will_set_reply() { return true; }
    virtual bool did_set_requestor() { return true; }
    virtual bool did_set_target() { return true; }
    virtual bool did_set_direction() { return true; }
    virtual bool did_set_command() { return true; }
    virtual bool did_set_reply() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    BTMonitorMsg *obj;

    bool obj_owned;
    int group;

    void communicate() {
      if (group!=0) return;
      if (yarp().canWrite()) {
        yarp().write(*this);
        clean();
      }
    }
    void mark_dirty() {
      is_dirty = true;
    }
    void mark_dirty_requestor() {
      if (is_dirty_requestor) return;
      dirty_count++;
      is_dirty_requestor = true;
      mark_dirty();
    }
    void mark_dirty_target() {
      if (is_dirty_target) return;
      dirty_count++;
      is_dirty_target = true;
      mark_dirty();
    }
    void mark_dirty_direction() {
      if (is_dirty_direction) return;
      dirty_count++;
      is_dirty_direction = true;
      mark_dirty();
    }
    void mark_dirty_command() {
      if (is_dirty_command) return;
      dirty_count++;
      is_dirty_command = true;
      mark_dirty();
    }
    void mark_dirty_reply() {
      if (is_dirty_reply) return;
      dirty_count++;
      is_dirty_reply = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_requestor = flag;
      is_dirty_target = flag;
      is_dirty_direction = flag;
      is_dirty_command = flag;
      is_dirty_reply = flag;
      dirty_count = flag ? 5 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_requestor;
    bool is_dirty_target;
    bool is_dirty_direction;
    bool is_dirty_command;
    bool is_dirty_reply;
  };
};

#endif
