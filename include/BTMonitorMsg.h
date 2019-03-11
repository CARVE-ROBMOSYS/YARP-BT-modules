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

class BTMonitorMsg;


class BTMonitorMsg : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::string source;
  std::string target;
  std::string event;

  // Default constructor
  BTMonitorMsg() : source(""), target(""), event("") {
  }

  // Constructor with field values
  BTMonitorMsg(const std::string& source,const std::string& target,const std::string& event) : source(source), target(target), event(event) {
  }

  // Copy constructor
  BTMonitorMsg(const BTMonitorMsg& __alt) : WirePortable(__alt)  {
    this->source = __alt.source;
    this->target = __alt.target;
    this->event = __alt.event;
  }

  // Assignment operator
  const BTMonitorMsg& operator = (const BTMonitorMsg& __alt) {
    this->source = __alt.source;
    this->target = __alt.target;
    this->event = __alt.event;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_source(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_source(const yarp::os::idl::WireWriter& writer) const;
  bool write_target(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_target(const yarp::os::idl::WireWriter& writer) const;
  bool write_event(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_event(const yarp::os::idl::WireWriter& writer) const;
  bool read_source(yarp::os::idl::WireReader& reader);
  bool nested_read_source(yarp::os::idl::WireReader& reader);
  bool read_target(yarp::os::idl::WireReader& reader);
  bool nested_read_target(yarp::os::idl::WireReader& reader);
  bool read_event(yarp::os::idl::WireReader& reader);
  bool nested_read_event(yarp::os::idl::WireReader& reader);

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
    void set_source(const std::string& source) {
      will_set_source();
      obj->source = source;
      mark_dirty_source();
      communicate();
      did_set_source();
    }
    void set_target(const std::string& target) {
      will_set_target();
      obj->target = target;
      mark_dirty_target();
      communicate();
      did_set_target();
    }
    void set_event(const std::string& event) {
      will_set_event();
      obj->event = event;
      mark_dirty_event();
      communicate();
      did_set_event();
    }
    const std::string& get_source() {
      return obj->source;
    }
    const std::string& get_target() {
      return obj->target;
    }
    const std::string& get_event() {
      return obj->event;
    }
    virtual bool will_set_source() { return true; }
    virtual bool will_set_target() { return true; }
    virtual bool will_set_event() { return true; }
    virtual bool did_set_source() { return true; }
    virtual bool did_set_target() { return true; }
    virtual bool did_set_event() { return true; }
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
    void mark_dirty_source() {
      if (is_dirty_source) return;
      dirty_count++;
      is_dirty_source = true;
      mark_dirty();
    }
    void mark_dirty_target() {
      if (is_dirty_target) return;
      dirty_count++;
      is_dirty_target = true;
      mark_dirty();
    }
    void mark_dirty_event() {
      if (is_dirty_event) return;
      dirty_count++;
      is_dirty_event = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_source = flag;
      is_dirty_target = flag;
      is_dirty_event = flag;
      dirty_count = flag ? 3 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_source;
    bool is_dirty_target;
    bool is_dirty_event;
  };
};

#endif
