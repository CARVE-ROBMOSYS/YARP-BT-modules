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
#include <string>

//YARP imports
#include <yarp/os/Port.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
#include <yarp/BT_wrappers/blackboard_client.h>

//#include <BTMonitorMsg.h>


using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::BT_wrappers;


class NavigationModule : public TickServer, public RFModule
{
private:
    Bottle              cmd, response;
    Property            navClientConfig;
    yarp::os::Port      goTo_port;
    yarp::os::Port      pathPlanner_port;

    BlackBoardClient    m_blackboardClient;

    INavigation2D       *iNav = 0;
    Map2DLocation       pos,curr_goal;
    PolyDriver          ddNavClient;

    //Gets the current navigation status
    NavigationStatusEnum status;
    double              linearTolerance{0.2}, angularTolerance{10};

public:

    // params contains the navigation parameters to configure the navigation server & c.
    ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        ReturnStatus ret = BT_ERROR;

        // Retrieve absolute position of 'target' from params.
        // target here is the Goal name (e.g. kitchen)

        // TODO: quick and dirty... check data are actually there and
        // they are correct

        Bottle & location = params.findGroup("location");
        yDebug() << "location is " << location.toString();

        yarp::dev::Nav2D::Map2DLocation desiredLoc;
        desiredLoc.map_id = location.find("map_id").asString();
        desiredLoc.x      = location.find("x").asDouble();
        desiredLoc.y      = location.find("y").asDouble();
        desiredLoc.theta  = location.find("theta").asDouble();

        double robotRadius = params.find("robotRadius").asDouble();
        yDebug() << "robotRadius is " << robotRadius;

        bool obstacle_avoidance = params.find("obstacle_avoidance").asBool();
        yDebug() << "obstacle_avoidance is " << (obstacle_avoidance ? "True":"False");

        bool valid = params.find("valid").asBool();
        yDebug() << "valid is " << valid;

        string type = params.find("type").asString();
        yDebug() << "type is " << type;

        Bottle config, reply;
        config.clear();
        config.addString("set_robot_radius");
        config.addFloat64(robotRadius);
        pathPlanner_port.write(config, reply);
        yInfo() << "response is " << reply.toString();

        config.clear();
        config.addString("set");
        config.addString("obstacle_stop");
        config.addInt32(obstacle_avoidance);
        goTo_port.write(config, reply);
        yInfo() << "response is " << reply.toString();

        config.clear();
        config.addString("set");
        config.addString("obstacle_avoidance");
        config.addInt32(obstacle_avoidance);
        goTo_port.write(config, reply);
        yInfo() << "response is " << reply.toString();

        // wait a bit to avoid concurrency issue
        yarp::os::Time::delay(0.2);

        // TODO: quick and dirty ... use enum instead?
        bool targetIsRoom = (type == "room") ? true : false;

        yInfo() << "Starting navigation toward desired location " << target.target << " (" << desiredLoc.toString() << ")";
        iNav->gotoTargetByAbsoluteLocation(desiredLoc);

        ret = BT_RUNNING;
        while( (ret == BT_RUNNING) && (!isHaltRequested(target)))
        {
            NavigationStatusEnum navStat;
            iNav->getNavigationStatus(navStat);

            yDebug() << "Navigation status is " << INavigation2DHelpers::statusToString(navStat);
            switch(navStat)
            {
                case navigation_status_aborted:
                case navigation_status_failing:
                {
                    return BT_FAILURE;
                } break;

                case navigation_status_error:
                {
                    return BT_ERROR;
                } break;
            }

            if(targetIsRoom)    // DO NOT collapse the two condition in a single if(isRoom && checkArea)!!!!
            {
                if(iNav->checkInsideArea(target.target))
                {
                    ret = BT_SUCCESS;
                    yDebug() << "Robot is inside target <" + target.target + "> area.";
                }
            }
            else
            {
                if(iNav->checkNearToLocation(desiredLoc, linearTolerance, angularTolerance))
                {
                    ret = BT_SUCCESS;
                    yDebug() << "Robot reached target <" + target.target + "> location.";
                }
            }
            yarp::os::Time::delay(0.1);
        }

        if(ret == BT_SUCCESS)
        {
            Property p;
            p.put("robotAt", Value(true));
            m_blackboardClient.setData(target.target, p);
            yDebug() << "setting robotAt to true for target " << target.target;
        }
        return (isHaltRequested(target) ? BT_HALTED : ret);
    }

    ReturnStatus request_halt(  const ActionID &target, const yarp::os::Property &params = {}) override
    {
        // halt function is called only if requested <target> is running, so it is safe to call stopNavigation
        // without double check
        iNav->stopNavigation();
        yInfo() << "Navigation toward target" << target.target << "Halted";
        return BT_HALTED;
    }

    bool configure(yarp::os::ResourceFinder &rf) override
    {
        navClientConfig.put("device", "navigation2DClient");
        navClientConfig.put("local", "/navigationClient");
        navClientConfig.put("navigation_server", "/navigationServer");
        navClientConfig.put("map_locations_server", "/mapServer");
        navClientConfig.put("localization_server", "/localizationServer");
        bool ok = ddNavClient.open(navClientConfig);
        if (!ok)
        {
            yError() << "Unable to open navigation2DClient device driver";
            return false;
        }

        ok = ddNavClient.view(iNav);

        if (!ok)
        {
            yError() << "Unable to open INavigation2D interface";
            return false;
        }

        yInfo() << "Navigation client successfully initialized";

        // For info purpose
        std::vector<std::string> locations;
        iNav->getLocationsList(locations);
        yInfo() << "List of known location";

        for(int i=0; i<locations.size(); i++)
            yInfo() << "  " << locations[i];


        // open a port to talk with pathPlanner for configuration
        pathPlanner_port.open("/SmartSoft/navigClient/pathPlanner/rpc");
        goTo_port.open("/SmartSoft/navigClient/goTo/rpc");

        bool conn = yarp::os::Network::connect(pathPlanner_port.getName(), "/robotPathPlanner/rpc");
        conn &= yarp::os::Network::connect(goTo_port.getName(), "/robotGoto/rpc");

        if(!conn)
        {
            yError() << " Cannot connect to pathPlanner or robotGoTo RPC ports";
            ddNavClient.close();
            return false;
        }

        std::string remoteBB_name =  rf.check("blackboard_port", Value("/blackboard"), "Port prefix for remote BlackBoard module").toString();
        m_blackboardClient.configureBlackBoardClient("", "navigation_module");
        m_blackboardClient.connectToBlackBoard(remoteBB_name);

        yInfo() << "Connections to RPCs done!";
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

    bool close()
    {
        ddNavClient.close();
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

    NavigationModule skill;
    skill.configure_TickServer("", "navigation_module", true);
    skill.runModule(rf);

    return 0;
}
