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
#include <iostream>                 // for std::cout
#include <chrono>

//YARP imports
#include <yarp/os/Network.h>        // for yarp::os::Network
#include <yarp/os/LogStream.h>      // for yError()
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IEncoders.h>

//behavior trees imports
#include <include/tick_server.h>

//device import
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>



//#include <yarp/os/RpcClient.h>
#include <eigen3/Eigen/Geometry> // for Eigen

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

class GotoInvPose : public TickServer, public RFModule
{
private:
    Bottle cmd, response;
    Property        navTestCfg;
    INavigation2D* iNav = 0;
    Map2DLocation pos,curr_goal;
    PolyDriver ddNavClient;

    //Gets the current navigation status
    NavigationStatusEnum status;

public:
    yarp::os::Port blackboard_port;

public:
    ReturnStatus execute_tick(const std::string& params = "") override
    {
        set_status(BT_RUNNING);

        yInfo() << "[GotoInvPose] Action started";
        cmd.clear();
        response.clear();
        cmd.addString("get");
        cmd.addString("InvPose");
        blackboard_port.write(cmd,response);
        std::string inv_pose_str =  response.get(0).asString();
        yInfo() << "[GotoInvPose] Going to" << inv_pose_str;


        std::string::size_type sz;     // alias of size_t
        std::replace(inv_pose_str.begin(), inv_pose_str.end(), ',', ' ');  // replace ',' by ' '

        vector<float> inv_pose_arr;
        stringstream ss(inv_pose_str);
        float temp;
        while (ss >> temp)
            inv_pose_arr.push_back(temp);

        iNav->gotoTargetByRelativeLocation(inv_pose_arr[0],inv_pose_arr[1], inv_pose_arr[2]);
        while(true)
        {
            iNav->getNavigationStatus(status);

            if (status == navigation_status_goal_reached) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        yInfo() << "[GotoInvPose] " << inv_pose_str;

        set_status(BT_SUCCESS);
        return BT_SUCCESS;
    }

    void execute_halt() override
    {
        iNav->stopNavigation();
        yInfo() << "[GotoInvPose] Halted" ;

    }

    bool configure(yarp::os::ResourceFinder &rf) override
    {

        navTestCfg.put("device", "navigation2DClient");
        navTestCfg.put("local", "/robotGotoExample");
        navTestCfg.put("navigation_server", "/robotGoto");
        navTestCfg.put("map_locations_server", "/mapServer");
        navTestCfg.put("localization_server", "/localizationServer");
        bool ok = ddNavClient.open(navTestCfg);
        if (!ok)
        {
            yError() << "Unable to open navigation2DClient device driver";
        }

        ok = ddNavClient.view(iNav);

        if (!ok)
        {
            yError() << "Unable to open INavigation2D interface";
        }

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
    // Message handler. Just echo all received messages.
    bool respond(const Bottle& command, Bottle& reply)
    {

        return true;
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

    GotoInvPose skill;
    skill.configure_tick_server("/GotoInvPose");
    skill.blackboard_port.open("/GotoInvPose/blackboard/rpc:o");
    skill.runModule(rf);

    return 0;
}
