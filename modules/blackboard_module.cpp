/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file condition_example_module.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

//standard imports
#include <map>
#include <mutex>
#include <chrono>
#include <iostream>
#include <algorithm>

//YARP imports
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
#include <yarp/BT_wrappers/BlackBoardWrapper.h>


using namespace yarp::os;
using namespace yarp::BT_wrappers;

class BlackBoard :  public BlackBoardWrapper,
                    public TickServer,
                    public RFModule
{
private:
    yarp::os::Port                  m_blackboard_port;  // a port to handle RPC  messages
    std::map<std::string, Property> m_storage;
    std::mutex                      m_mutex;

public:
    bool request_initialize() override
    {
        initializeValues();
        return true;
    }

    ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        ReturnStatus ret = BT_ERROR;
        yInfo() << "Request_tick with target " << target.target << " and params " << params.toString();

        // get <target> from param
        if(target.target == "")
        {
            yError() << "Missing <flag> parameter from tick request";
            return BT_ERROR;
        }
        // if target is missing from blackboard I'll get an empty property ...
        Property data = getData(target.target);

        // get <flag> from param
        if(!params.check("flag"))
        {
            yError() << "Missing <flag> parameter from tick request";
            return BT_ERROR;
        }
        std::string flagName = params.find("flag").asString();

        // finally get the actual value from data
        if(!data.check(flagName))
        {
            yError() << std::string("Requested flag <") + flagName + " was not found in the blackboard for target <" + target.target + ">.";
            return BT_ERROR;
        }

        ReturnStatusVocab a;
        data.find(flagName).asBool() ? ret = BT_SUCCESS : ret = BT_FAILURE;
        yInfo() << "result is " << a.toString(ret);
        return ret;
    }

    ReturnStatus request_halt(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        yInfo() << "BlackBoard received hatl request. Nothing to do";
        return BT_HALTED;
    }

    yarp::os::Property getData(const std::string& target) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        Property p = m_storage[target];
        yInfo() << "getData with target " << p.toString();
        return m_storage[target];
    }

    std::vector<std::string> listTarget() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> ret;
        for(auto entry : m_storage)
        {
            yInfo() << entry.first << entry.second.toString();
            ret.emplace_back(entry.first);
        }
        return ret;
    }

    void clearData() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_storage.clear();
    }

    void initializeData() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_storage.clear();
        initializeValues();
    }


    bool setData(const std::string& target, const yarp::os::Property& datum) override
    {
        /* Using fromString(toString) is the only way known to merge two properties together.
         * In case the pair <key, value> exists only in the lhs, it'll be kept as is
         * In case the pair <key, value> exists only in the rhs, it'll be copied into the lhs
         * In case the pair <key, value> exists in both lhs and rhs, the one in the lhs will
         * overwrite the one in lhs.
         *
         * See https://www.yarp.it/classyarp_1_1os_1_1Property.html for documentation
         */
        std::lock_guard<std::mutex> lock(m_mutex);

        yInfo() << "setData with target " << target << " and params " << datum.toString();

        m_storage[target].fromString(datum.toString(), false);
        return true;
    }

    double getPeriod()
    {
        // module periodicity (seconds), called implicitly by the module.
        return 1.0;
    }

    // This is our main function. Will be called periodically every getPeriod() seconds
    bool updateModule()
    {
        //cout << "[" << count << "]" << " updateModule..." << endl;
        return true;
    }

    bool configure(yarp::os::ResourceFinder &rf)
       {
           // optional, attach a port to the module
           // so that messages received from the port are redirected
           // to the respond method
           m_blackboard_port.open("/blackboard/rpc:s");

           // Attach BlackBoard thrift message parser
           BlackBoardWrapper::yarp().attachAsServer(m_blackboard_port);

           return true;
       }

    bool interruptModule()
    {
        return true;
    }

    // Close function, to perform cleanup.
    bool close()
    {
        m_blackboard_port.close();
        return true;
    }

    void initializeValues()
    {
        Property p;
        {
            p.put("known",              Value(true));
            p.put("type",               Value("room"));
            p.put("valid",              Value(true));
            p.put("robotAt",            Value(false));
            p.put("robotRadius",        Value(0.35f));
            p.put("obstacle_avoidance", Value(true));
            Property &loc1 = p.addGroup("location");
            loc1.put("map_id", "sanquirico");
            loc1.put("x",      10.00);
            loc1.put("y",       2.02);
            loc1.put("theta",   0.00);
            setData("kitchen", p);
        }

        {   // Location values will be computed by an external module.
            // This entry is useful to store the other nav parameters.
            p.clear();
            p.put("known",              Value(true));
            p.put("type",               Value("location"));
            p.put("valid",              Value(false));
            p.put("robotAt",            Value(false));
            p.put("robotRadius",        Value(0.1f));
            p.put("obstacle_avoidance", Value(false));
            Property &loc = p.addGroup("location");
            loc.put("map_id", "sanquirico");
            loc.put("x",      0.0);
            loc.put("y",      0.0);
            loc.put("theta",  0.0);
            setData("invPose", p);
        }

        {
            p.clear();
            p.put("known",              Value(true));
            p.put("type",               Value("location"));
            p.put("valid",              Value(false));
            p.put("robotAt",            Value(false));
            p.put("robotRadius",        Value(0.35f));
            p.put("obstacle_avoidance", Value(true));
            Property &loc = p.addGroup("location");
            loc.put("map_id", "sanquirico");
            loc.put("x",        10.0);
            loc.put("y",        2.02);
            loc.put("theta",  -14.56);
            setData("findBottle", p);
        }

        {   // Location values will be computed by an external module.
            // This entry is useful to store the other nav parameters.
            p.clear();
            p.put("grasped",            Value(false));
            setData("bottle", p);
        }

//        set("homeArms",                 Value("(joint ( "
//                                              "(left_arm  45 60 10 80 18) "
//                                              "(right_arm 45 60 10 80 18) "
//                                              "(torso 0.01) "
//                                              ") KeepArmsForGrasp)"));

    }
};

int main(int argc, char * argv[])
{
    /* initialize yarp network */
    yarp::os::Network yarp;
    if (!yarp::os::Network::checkNetwork(5.0))
    {
        yError() << " YARP server not available!";
        return EXIT_FAILURE;
    }

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    BlackBoard blackboard;
    blackboard.configure_TickServer("", "blackboard");

    blackboard.initializeValues();
    blackboard.runModule(rf);
    return 0;
}
