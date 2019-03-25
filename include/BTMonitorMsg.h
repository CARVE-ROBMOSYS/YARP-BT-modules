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
  std::string skill;
  std::string event;

  // Default constructor
  BTMonitorMsg() : skill(""), event("") {
  }

  // Constructor with field values
  BTMonitorMsg(const std::string& skill,const std::string& event) : skill(skill), event(event) {
  }

  // Copy constructor
  BTMonitorMsg(const BTMonitorMsg& __alt) : WirePortable(__alt)  {
    this->skill = __alt.skill;
    this->event = __alt.event;
  }

  // Assignment operator
  const BTMonitorMsg& operator = (const BTMonitorMsg& __alt) {
    this->skill = __alt.skill;
    this->event = __alt.event;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_skill(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_skill(const yarp::os::idl::WireWriter& writer) const;
  bool write_event(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_event(const yarp::os::idl::WireWriter& writer) const;
  bool read_skill(yarp::os::idl::WireReader& reader);
  bool nested_read_skill(yarp::os::idl::WireReader& reader);
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
    void set_skill(const std::string& skill) {
      will_set_skill();
      obj->skill = skill;
      mark_dirty_skill();
      communicate();
      did_set_skill();
    }
    void set_event(const std::string& event) {
      will_set_event();
      obj->event = event;
      mark_dirty_event();
      communicate();
      did_set_event();
    }
    const std::string& get_skill() {
      return obj->skill;
    }
    const std::string& get_event() {
      return obj->event;
    }
    virtual bool will_set_skill() { return true; }
    virtual bool will_set_event() { return true; }
    virtual bool did_set_skill() { return true; }
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
    void mark_dirty_skill() {
      if (is_dirty_skill) return;
      dirty_count++;
      is_dirty_skill = true;
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
      is_dirty_skill = flag;
      is_dirty_event = flag;
      dirty_count = flag ? 2 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_skill;
    bool is_dirty_event;
  };
};

#endif
