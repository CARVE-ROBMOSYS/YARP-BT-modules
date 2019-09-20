/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// Autogenerated by Thrift Compiler (0.12.0-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <yarp/BT_wrappers/MonitorMsg.h>

namespace yarp {
namespace BT_wrappers {

// Default constructor
MonitorMsg::MonitorMsg() :
        WirePortable(),
        skill(""),
        target(""),
        event("")
{
}

// Constructor with field values
MonitorMsg::MonitorMsg(const std::string& skill,
                       const std::string& target,
                       const std::string& event) :
        WirePortable(),
        skill(skill),
        target(target),
        event(event)
{
}

// Read structure on a Wire
bool MonitorMsg::read(yarp::os::idl::WireReader& reader)
{
    if (!read_skill(reader)) {
        return false;
    }
    if (!read_target(reader)) {
        return false;
    }
    if (!read_event(reader)) {
        return false;
    }
    return !reader.isError();
}

// Read structure on a Connection
bool MonitorMsg::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(3)) {
        return false;
    }
    return read(reader);
}

// Write structure on a Wire
bool MonitorMsg::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!write_skill(writer)) {
        return false;
    }
    if (!write_target(writer)) {
        return false;
    }
    if (!write_event(writer)) {
        return false;
    }
    return !writer.isError();
}

// Write structure on a Connection
bool MonitorMsg::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(3)) {
        return false;
    }
    return write(writer);
}

// Convert to a printable string
std::string MonitorMsg::toString() const
{
    yarp::os::Bottle b;
    b.read(*this);
    return b.toString();
}

// Editor: default constructor
MonitorMsg::Editor::Editor()
{
    group = 0;
    obj_owned = true;
    obj = new MonitorMsg;
    dirty_flags(false);
    yarp().setOwner(*this);
}

// Editor: constructor with base class
MonitorMsg::Editor::Editor(MonitorMsg& obj)
{
    group = 0;
    obj_owned = false;
    edit(obj, false);
    yarp().setOwner(*this);
}

// Editor: destructor
MonitorMsg::Editor::~Editor()
{
    if (obj_owned) {
        delete obj;
    }
}

// Editor: edit
bool MonitorMsg::Editor::edit(MonitorMsg& obj, bool dirty)
{
    if (obj_owned) {
        delete this->obj;
    }
    this->obj = &obj;
    obj_owned = false;
    dirty_flags(dirty);
    return true;
}

// Editor: validity check
bool MonitorMsg::Editor::isValid() const
{
    return obj != nullptr;
}

// Editor: state
MonitorMsg& MonitorMsg::Editor::state()
{
    return *obj;
}

// Editor: grouping begin
void MonitorMsg::Editor::start_editing()
{
    group++;
}

// Editor: grouping end
void MonitorMsg::Editor::stop_editing()
{
    group--;
    if (group == 0 && is_dirty) {
        communicate();
    }
}
// Editor: skill setter
void MonitorMsg::Editor::set_skill(const std::string& skill)
{
    will_set_skill();
    obj->skill = skill;
    mark_dirty_skill();
    communicate();
    did_set_skill();
}

// Editor: skill getter
const std::string& MonitorMsg::Editor::get_skill() const
{
    return obj->skill;
}

// Editor: skill will_set
bool MonitorMsg::Editor::will_set_skill()
{
    return true;
}

// Editor: skill did_set
bool MonitorMsg::Editor::did_set_skill()
{
    return true;
}

// Editor: target setter
void MonitorMsg::Editor::set_target(const std::string& target)
{
    will_set_target();
    obj->target = target;
    mark_dirty_target();
    communicate();
    did_set_target();
}

// Editor: target getter
const std::string& MonitorMsg::Editor::get_target() const
{
    return obj->target;
}

// Editor: target will_set
bool MonitorMsg::Editor::will_set_target()
{
    return true;
}

// Editor: target did_set
bool MonitorMsg::Editor::did_set_target()
{
    return true;
}

// Editor: event setter
void MonitorMsg::Editor::set_event(const std::string& event)
{
    will_set_event();
    obj->event = event;
    mark_dirty_event();
    communicate();
    did_set_event();
}

// Editor: event getter
const std::string& MonitorMsg::Editor::get_event() const
{
    return obj->event;
}

// Editor: event will_set
bool MonitorMsg::Editor::will_set_event()
{
    return true;
}

// Editor: event did_set
bool MonitorMsg::Editor::did_set_event()
{
    return true;
}

// Editor: clean
void MonitorMsg::Editor::clean()
{
    dirty_flags(false);
}

// Editor: read
bool MonitorMsg::Editor::read(yarp::os::ConnectionReader& connection)
{
    if (!isValid()) {
        return false;
    }
    yarp::os::idl::WireReader reader(connection);
    reader.expectAccept();
    if (!reader.readListHeader()) {
        return false;
    }
    int len = reader.getLength();
    if (len == 0) {
        yarp::os::idl::WireWriter writer(reader);
        if (writer.isNull()) {
            return true;
        }
        if (!writer.writeListHeader(1)) {
            return false;
        }
        writer.writeString("send: 'help' or 'patch (param1 val1) (param2 val2)'");
        return true;
    }
    std::string tag;
    if (!reader.readString(tag)) {
        return false;
    }
    if (tag == "help") {
        yarp::os::idl::WireWriter writer(reader);
        if (writer.isNull()) {
            return true;
        }
        if (!writer.writeListHeader(2)) {
            return false;
        }
        if (!writer.writeTag("many", 1, 0)) {
            return false;
        }
        if (reader.getLength() > 0) {
            std::string field;
            if (!reader.readString(field)) {
                return false;
            }
            if (field == "skill") {
                if (!writer.writeListHeader(1)) {
                    return false;
                }
                if (!writer.writeString("std::string skill")) {
                    return false;
                }
            }
            if (field == "target") {
                if (!writer.writeListHeader(1)) {
                    return false;
                }
                if (!writer.writeString("std::string target")) {
                    return false;
                }
            }
            if (field == "event") {
                if (!writer.writeListHeader(1)) {
                    return false;
                }
                if (!writer.writeString("std::string event")) {
                    return false;
                }
            }
        }
        if (!writer.writeListHeader(4)) {
            return false;
        }
        writer.writeString("*** Available fields:");
        writer.writeString("skill");
        writer.writeString("target");
        writer.writeString("event");
        return true;
    }
    bool nested = true;
    bool have_act = false;
    if (tag != "patch") {
        if (((len - 1) % 2) != 0) {
            return false;
        }
        len = 1 + ((len - 1) / 2);
        nested = false;
        have_act = true;
    }
    for (int i = 1; i < len; ++i) {
        if (nested && !reader.readListHeader(3)) {
            return false;
        }
        std::string act;
        std::string key;
        if (have_act) {
            act = tag;
        } else if (!reader.readString(act)) {
            return false;
        }
        if (!reader.readString(key)) {
            return false;
        }
        if (key == "skill") {
            will_set_skill();
            if (!obj->nested_read_skill(reader)) {
                return false;
            }
            did_set_skill();
        } else if (key == "target") {
            will_set_target();
            if (!obj->nested_read_target(reader)) {
                return false;
            }
            did_set_target();
        } else if (key == "event") {
            will_set_event();
            if (!obj->nested_read_event(reader)) {
                return false;
            }
            did_set_event();
        } else {
            // would be useful to have a fallback here
        }
    }
    reader.accept();
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) {
        return true;
    }
    writer.writeListHeader(1);
    writer.writeVocab(yarp::os::createVocab('o', 'k'));
    return true;
}

// Editor: write
bool MonitorMsg::Editor::write(yarp::os::ConnectionWriter& connection) const
{
    if (!isValid()) {
        return false;
    }
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(dirty_count + 1)) {
        return false;
    }
    if (!writer.writeString("patch")) {
        return false;
    }
    if (is_dirty_skill) {
        if (!writer.writeListHeader(3)) {
            return false;
        }
        if (!writer.writeString("set")) {
            return false;
        }
        if (!writer.writeString("skill")) {
            return false;
        }
        if (!obj->nested_write_skill(writer)) {
            return false;
        }
    }
    if (is_dirty_target) {
        if (!writer.writeListHeader(3)) {
            return false;
        }
        if (!writer.writeString("set")) {
            return false;
        }
        if (!writer.writeString("target")) {
            return false;
        }
        if (!obj->nested_write_target(writer)) {
            return false;
        }
    }
    if (is_dirty_event) {
        if (!writer.writeListHeader(3)) {
            return false;
        }
        if (!writer.writeString("set")) {
            return false;
        }
        if (!writer.writeString("event")) {
            return false;
        }
        if (!obj->nested_write_event(writer)) {
            return false;
        }
    }
    return !writer.isError();
}

// Editor: send if possible
void MonitorMsg::Editor::communicate()
{
    if (group != 0) {
        return;
    }
    if (yarp().canWrite()) {
        yarp().write(*this);
        clean();
    }
}

// Editor: mark dirty overall
void MonitorMsg::Editor::mark_dirty()
{
    is_dirty = true;
}

// Editor: skill mark_dirty
void MonitorMsg::Editor::mark_dirty_skill()
{
    if (is_dirty_skill) {
        return;
    }
    dirty_count++;
    is_dirty_skill = true;
    mark_dirty();
}

// Editor: target mark_dirty
void MonitorMsg::Editor::mark_dirty_target()
{
    if (is_dirty_target) {
        return;
    }
    dirty_count++;
    is_dirty_target = true;
    mark_dirty();
}

// Editor: event mark_dirty
void MonitorMsg::Editor::mark_dirty_event()
{
    if (is_dirty_event) {
        return;
    }
    dirty_count++;
    is_dirty_event = true;
    mark_dirty();
}

// Editor: dirty_flags
void MonitorMsg::Editor::dirty_flags(bool flag)
{
    is_dirty = flag;
    is_dirty_skill = flag;
    is_dirty_target = flag;
    is_dirty_event = flag;
    dirty_count = flag ? 3 : 0;
}

// read skill field
bool MonitorMsg::read_skill(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(skill)) {
        reader.fail();
        return false;
    }
    return true;
}

// write skill field
bool MonitorMsg::write_skill(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(skill)) {
        return false;
    }
    return true;
}

// read (nested) skill field
bool MonitorMsg::nested_read_skill(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(skill)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) skill field
bool MonitorMsg::nested_write_skill(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(skill)) {
        return false;
    }
    return true;
}

// read target field
bool MonitorMsg::read_target(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(target)) {
        reader.fail();
        return false;
    }
    return true;
}

// write target field
bool MonitorMsg::write_target(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(target)) {
        return false;
    }
    return true;
}

// read (nested) target field
bool MonitorMsg::nested_read_target(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(target)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) target field
bool MonitorMsg::nested_write_target(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(target)) {
        return false;
    }
    return true;
}

// read event field
bool MonitorMsg::read_event(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(event)) {
        reader.fail();
        return false;
    }
    return true;
}

// write event field
bool MonitorMsg::write_event(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(event)) {
        return false;
    }
    return true;
}

// read (nested) event field
bool MonitorMsg::nested_read_event(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(event)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) event field
bool MonitorMsg::nested_write_event(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(event)) {
        return false;
    }
    return true;
}

} // namespace yarp
} // namespace BT_wrappers
