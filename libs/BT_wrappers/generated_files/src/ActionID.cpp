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

#include <yarp/BT_wrappers/ActionID.h>

namespace yarp {
namespace BT_wrappers {

// Default constructor
ActionID::ActionID() :
        WirePortable(),
        target(""),
        resources(""),
        action_ID(0),
        BT_filename("")
{
}

// Constructor with field values
ActionID::ActionID(const std::string& target,
                   const std::string& resources,
                   const std::int32_t action_ID,
                   const std::string& BT_filename) :
        WirePortable(),
        target(target),
        resources(resources),
        action_ID(action_ID),
        BT_filename(BT_filename)
{
}

// Read structure on a Wire
bool ActionID::read(yarp::os::idl::WireReader& reader)
{
    if (!read_target(reader)) {
        return false;
    }
    if (!read_resources(reader)) {
        return false;
    }
    if (!read_action_ID(reader)) {
        return false;
    }
    if (!read_BT_filename(reader)) {
        return false;
    }
    return !reader.isError();
}

// Read structure on a Connection
bool ActionID::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(4)) {
        return false;
    }
    return read(reader);
}

// Write structure on a Wire
bool ActionID::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!write_target(writer)) {
        return false;
    }
    if (!write_resources(writer)) {
        return false;
    }
    if (!write_action_ID(writer)) {
        return false;
    }
    if (!write_BT_filename(writer)) {
        return false;
    }
    return !writer.isError();
}

// Write structure on a Connection
bool ActionID::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(4)) {
        return false;
    }
    return write(writer);
}

// Convert to a printable string
std::string ActionID::toString() const
{
    yarp::os::Bottle b;
    b.read(*this);
    return b.toString();
}

// Editor: default constructor
ActionID::Editor::Editor()
{
    group = 0;
    obj_owned = true;
    obj = new ActionID;
    dirty_flags(false);
    yarp().setOwner(*this);
}

// Editor: constructor with base class
ActionID::Editor::Editor(ActionID& obj)
{
    group = 0;
    obj_owned = false;
    edit(obj, false);
    yarp().setOwner(*this);
}

// Editor: destructor
ActionID::Editor::~Editor()
{
    if (obj_owned) {
        delete obj;
    }
}

// Editor: edit
bool ActionID::Editor::edit(ActionID& obj, bool dirty)
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
bool ActionID::Editor::isValid() const
{
    return obj != nullptr;
}

// Editor: state
ActionID& ActionID::Editor::state()
{
    return *obj;
}

// Editor: grouping begin
void ActionID::Editor::start_editing()
{
    group++;
}

// Editor: grouping end
void ActionID::Editor::stop_editing()
{
    group--;
    if (group == 0 && is_dirty) {
        communicate();
    }
}
// Editor: target setter
void ActionID::Editor::set_target(const std::string& target)
{
    will_set_target();
    obj->target = target;
    mark_dirty_target();
    communicate();
    did_set_target();
}

// Editor: target getter
const std::string& ActionID::Editor::get_target() const
{
    return obj->target;
}

// Editor: target will_set
bool ActionID::Editor::will_set_target()
{
    return true;
}

// Editor: target did_set
bool ActionID::Editor::did_set_target()
{
    return true;
}

// Editor: resources setter
void ActionID::Editor::set_resources(const std::string& resources)
{
    will_set_resources();
    obj->resources = resources;
    mark_dirty_resources();
    communicate();
    did_set_resources();
}

// Editor: resources getter
const std::string& ActionID::Editor::get_resources() const
{
    return obj->resources;
}

// Editor: resources will_set
bool ActionID::Editor::will_set_resources()
{
    return true;
}

// Editor: resources did_set
bool ActionID::Editor::did_set_resources()
{
    return true;
}

// Editor: action_ID setter
void ActionID::Editor::set_action_ID(const std::int32_t action_ID)
{
    will_set_action_ID();
    obj->action_ID = action_ID;
    mark_dirty_action_ID();
    communicate();
    did_set_action_ID();
}

// Editor: action_ID getter
std::int32_t ActionID::Editor::get_action_ID() const
{
    return obj->action_ID;
}

// Editor: action_ID will_set
bool ActionID::Editor::will_set_action_ID()
{
    return true;
}

// Editor: action_ID did_set
bool ActionID::Editor::did_set_action_ID()
{
    return true;
}

// Editor: BT_filename setter
void ActionID::Editor::set_BT_filename(const std::string& BT_filename)
{
    will_set_BT_filename();
    obj->BT_filename = BT_filename;
    mark_dirty_BT_filename();
    communicate();
    did_set_BT_filename();
}

// Editor: BT_filename getter
const std::string& ActionID::Editor::get_BT_filename() const
{
    return obj->BT_filename;
}

// Editor: BT_filename will_set
bool ActionID::Editor::will_set_BT_filename()
{
    return true;
}

// Editor: BT_filename did_set
bool ActionID::Editor::did_set_BT_filename()
{
    return true;
}

// Editor: clean
void ActionID::Editor::clean()
{
    dirty_flags(false);
}

// Editor: read
bool ActionID::Editor::read(yarp::os::ConnectionReader& connection)
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
            if (field == "target") {
                if (!writer.writeListHeader(1)) {
                    return false;
                }
                if (!writer.writeString("std::string target")) {
                    return false;
                }
            }
            if (field == "resources") {
                if (!writer.writeListHeader(1)) {
                    return false;
                }
                if (!writer.writeString("std::string resources")) {
                    return false;
                }
            }
            if (field == "action_ID") {
                if (!writer.writeListHeader(1)) {
                    return false;
                }
                if (!writer.writeString("std::int32_t action_ID")) {
                    return false;
                }
            }
            if (field == "BT_filename") {
                if (!writer.writeListHeader(1)) {
                    return false;
                }
                if (!writer.writeString("std::string BT_filename")) {
                    return false;
                }
            }
        }
        if (!writer.writeListHeader(5)) {
            return false;
        }
        writer.writeString("*** Available fields:");
        writer.writeString("target");
        writer.writeString("resources");
        writer.writeString("action_ID");
        writer.writeString("BT_filename");
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
        if (key == "target") {
            will_set_target();
            if (!obj->nested_read_target(reader)) {
                return false;
            }
            did_set_target();
        } else if (key == "resources") {
            will_set_resources();
            if (!obj->nested_read_resources(reader)) {
                return false;
            }
            did_set_resources();
        } else if (key == "action_ID") {
            will_set_action_ID();
            if (!obj->nested_read_action_ID(reader)) {
                return false;
            }
            did_set_action_ID();
        } else if (key == "BT_filename") {
            will_set_BT_filename();
            if (!obj->nested_read_BT_filename(reader)) {
                return false;
            }
            did_set_BT_filename();
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
bool ActionID::Editor::write(yarp::os::ConnectionWriter& connection) const
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
    if (is_dirty_resources) {
        if (!writer.writeListHeader(3)) {
            return false;
        }
        if (!writer.writeString("set")) {
            return false;
        }
        if (!writer.writeString("resources")) {
            return false;
        }
        if (!obj->nested_write_resources(writer)) {
            return false;
        }
    }
    if (is_dirty_action_ID) {
        if (!writer.writeListHeader(3)) {
            return false;
        }
        if (!writer.writeString("set")) {
            return false;
        }
        if (!writer.writeString("action_ID")) {
            return false;
        }
        if (!obj->nested_write_action_ID(writer)) {
            return false;
        }
    }
    if (is_dirty_BT_filename) {
        if (!writer.writeListHeader(3)) {
            return false;
        }
        if (!writer.writeString("set")) {
            return false;
        }
        if (!writer.writeString("BT_filename")) {
            return false;
        }
        if (!obj->nested_write_BT_filename(writer)) {
            return false;
        }
    }
    return !writer.isError();
}

// Editor: send if possible
void ActionID::Editor::communicate()
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
void ActionID::Editor::mark_dirty()
{
    is_dirty = true;
}

// Editor: target mark_dirty
void ActionID::Editor::mark_dirty_target()
{
    if (is_dirty_target) {
        return;
    }
    dirty_count++;
    is_dirty_target = true;
    mark_dirty();
}

// Editor: resources mark_dirty
void ActionID::Editor::mark_dirty_resources()
{
    if (is_dirty_resources) {
        return;
    }
    dirty_count++;
    is_dirty_resources = true;
    mark_dirty();
}

// Editor: action_ID mark_dirty
void ActionID::Editor::mark_dirty_action_ID()
{
    if (is_dirty_action_ID) {
        return;
    }
    dirty_count++;
    is_dirty_action_ID = true;
    mark_dirty();
}

// Editor: BT_filename mark_dirty
void ActionID::Editor::mark_dirty_BT_filename()
{
    if (is_dirty_BT_filename) {
        return;
    }
    dirty_count++;
    is_dirty_BT_filename = true;
    mark_dirty();
}

// Editor: dirty_flags
void ActionID::Editor::dirty_flags(bool flag)
{
    is_dirty = flag;
    is_dirty_target = flag;
    is_dirty_resources = flag;
    is_dirty_action_ID = flag;
    is_dirty_BT_filename = flag;
    dirty_count = flag ? 4 : 0;
}

// read target field
bool ActionID::read_target(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(target)) {
        reader.fail();
        return false;
    }
    return true;
}

// write target field
bool ActionID::write_target(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(target)) {
        return false;
    }
    return true;
}

// read (nested) target field
bool ActionID::nested_read_target(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(target)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) target field
bool ActionID::nested_write_target(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(target)) {
        return false;
    }
    return true;
}

// read resources field
bool ActionID::read_resources(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(resources)) {
        reader.fail();
        return false;
    }
    return true;
}

// write resources field
bool ActionID::write_resources(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(resources)) {
        return false;
    }
    return true;
}

// read (nested) resources field
bool ActionID::nested_read_resources(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(resources)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) resources field
bool ActionID::nested_write_resources(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(resources)) {
        return false;
    }
    return true;
}

// read action_ID field
bool ActionID::read_action_ID(yarp::os::idl::WireReader& reader)
{
    if (!reader.readI32(action_ID)) {
        reader.fail();
        return false;
    }
    return true;
}

// write action_ID field
bool ActionID::write_action_ID(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeI32(action_ID)) {
        return false;
    }
    return true;
}

// read (nested) action_ID field
bool ActionID::nested_read_action_ID(yarp::os::idl::WireReader& reader)
{
    if (!reader.readI32(action_ID)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) action_ID field
bool ActionID::nested_write_action_ID(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeI32(action_ID)) {
        return false;
    }
    return true;
}

// read BT_filename field
bool ActionID::read_BT_filename(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(BT_filename)) {
        reader.fail();
        return false;
    }
    return true;
}

// write BT_filename field
bool ActionID::write_BT_filename(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(BT_filename)) {
        return false;
    }
    return true;
}

// read (nested) BT_filename field
bool ActionID::nested_read_BT_filename(yarp::os::idl::WireReader& reader)
{
    if (!reader.readString(BT_filename)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) BT_filename field
bool ActionID::nested_write_BT_filename(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(BT_filename)) {
        return false;
    }
    return true;
}

} // namespace yarp
} // namespace BT_wrappers
