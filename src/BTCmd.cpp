/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <BTCmd.h>
#include <yarp/os/idl/WireTypes.h>



class BTCmd_request_tick : public yarp::os::Portable {
public:
  std::string params;
  ReturnStatus _return;
  void init(const std::string& params);
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

class BTCmd_request_status : public yarp::os::Portable {
public:
  ReturnStatus _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

class BTCmd_request_halt : public yarp::os::Portable {
public:
  ReturnStatus _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

bool BTCmd_request_tick::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("request_tick",1,2)) return false;
  if (!writer.writeString(params)) return false;
  return true;
}

bool BTCmd_request_tick::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  int32_t ecast0;
  ReturnStatusVocab cvrt1;
  if (!reader.readEnum(ecast0,cvrt1)) {
    reader.fail();
    return false;
  } else {
    _return = (ReturnStatus)ecast0;
  }
  return true;
}

void BTCmd_request_tick::init(const std::string& params) {
  _return = (ReturnStatus)0;
  this->params = params;
}

bool BTCmd_request_status::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("request_status",1,2)) return false;
  return true;
}

bool BTCmd_request_status::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  int32_t ecast2;
  ReturnStatusVocab cvrt3;
  if (!reader.readEnum(ecast2,cvrt3)) {
    reader.fail();
    return false;
  } else {
    _return = (ReturnStatus)ecast2;
  }
  return true;
}

void BTCmd_request_status::init() {
  _return = (ReturnStatus)0;
}

bool BTCmd_request_halt::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("request_halt",1,2)) return false;
  return true;
}

bool BTCmd_request_halt::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  int32_t ecast4;
  ReturnStatusVocab cvrt5;
  if (!reader.readEnum(ecast4,cvrt5)) {
    reader.fail();
    return false;
  } else {
    _return = (ReturnStatus)ecast4;
  }
  return true;
}

void BTCmd_request_halt::init() {
  _return = (ReturnStatus)0;
}

BTCmd::BTCmd() {
  yarp().setOwner(*this);
}
ReturnStatus BTCmd::request_tick(const std::string& params) {
  ReturnStatus _return = (ReturnStatus)0;
  BTCmd_request_tick helper;
  helper.init(params);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","ReturnStatus BTCmd::request_tick(const std::string& params)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
ReturnStatus BTCmd::request_status() {
  ReturnStatus _return = (ReturnStatus)0;
  BTCmd_request_status helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","ReturnStatus BTCmd::request_status()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
ReturnStatus BTCmd::request_halt() {
  ReturnStatus _return = (ReturnStatus)0;
  BTCmd_request_halt helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","ReturnStatus BTCmd::request_halt()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool BTCmd::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  std::string tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "request_tick") {
      std::string params;
      if (!reader.readString(params)) {
        params = "";
      }
      ReturnStatus _return;
      _return = request_tick(params);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32((int32_t)_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "request_status") {
      ReturnStatus _return;
      _return = request_status();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32((int32_t)_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "request_halt") {
      ReturnStatus _return;
      _return = request_halt();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32((int32_t)_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "help") {
      std::string functionName;
      if (!reader.readString(functionName)) {
        functionName = "--all";
      }
      std::vector<std::string> _return=help(functionName);
      yarp::os::idl::WireWriter writer(reader);
        if (!writer.isNull()) {
          if (!writer.writeListHeader(2)) return false;
          if (!writer.writeTag("many",1, 0)) return false;
          if (!writer.writeListBegin(BOTTLE_TAG_INT32, static_cast<uint32_t>(_return.size()))) return false;
          std::vector<std::string> ::iterator _iterHelp;
          for (_iterHelp = _return.begin(); _iterHelp != _return.end(); ++_iterHelp)
          {
            if (!writer.writeString(*_iterHelp)) return false;
           }
          if (!writer.writeListEnd()) return false;
        }
      reader.accept();
      return true;
    }
    if (reader.noMore()) { reader.fail(); return false; }
    std::string next_tag = reader.readTag();
    if (next_tag=="") break;
    tag = tag + "_" + next_tag;
  }
  return false;
}

std::vector<std::string> BTCmd::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("request_tick");
    helpString.push_back("request_status");
    helpString.push_back("request_halt");
    helpString.push_back("help");
  }
  else {
    if (functionName=="request_tick") {
      helpString.push_back("ReturnStatus request_tick(const std::string& params = \"\") ");
    }
    if (functionName=="request_status") {
      helpString.push_back("ReturnStatus request_status() ");
    }
    if (functionName=="request_halt") {
      helpString.push_back("ReturnStatus request_halt() ");
    }
    if (functionName=="help") {
      helpString.push_back("std::vector<std::string> help(const std::string& functionName=\"--all\")");
      helpString.push_back("Return list of available commands, or help message for a specific function");
      helpString.push_back("@param functionName name of command for which to get a detailed description. If none or '--all' is provided, print list of available commands");
      helpString.push_back("@return list of strings (one string per line)");
    }
  }
  if ( helpString.empty()) helpString.push_back("Command not found");
  return helpString;
}


