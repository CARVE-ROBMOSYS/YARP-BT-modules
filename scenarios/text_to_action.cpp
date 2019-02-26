/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file text_to_action.cpp
 * @authors: Marco Randazzo <marco.randazzo@iit.it>
 * @brief - - - 
 * @remarks - - -
 */

//standard imports
#include <cstdlib>
#include <string>
#include <algorithm>

//YARP imports
#include <yarp/os/all.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace std;

/****************************************************************/
class TextToAction : public RFModule
{
    private:
    Port      speechTranscription_port;         // should be connected to ...
    RpcClient blackboard_port;                  // should be connected to /blackboard/rpc:i
    std::string string1;
    std::string string2;
    PolyDriver ddNavClient;
    INavigation2D* iNav;

    public:
    TextToAction ()
    {
        string1 = "this is the kitchen";
        string1 = "forget room";
        iNav = 0;
    }

    bool writeToBlackboard(bool flag_kitchen)
    {
        if (blackboard_port.getOutputCount() < 1)
        {
            yError() << "writeObjectPositionToBlackboard: no connection to blackboard";
            return false;
        }

        {
            Bottle cmd, reply;
            cmd.clear();
            reply.clear();
            cmd.addString("set");
            cmd.addString("RoomKnown");
            if (flag_kitchen) cmd.addString("True");
            else cmd.addString("False");
            blackboard_port.write(cmd, reply);

            if (reply.size() != 1)
            {
                yError() << "writeOnBlackboard: invalid answer from blackboard: " << reply.toString();
                return false;
            }

            if (reply.get(0).asInt() != 1)
            {
                yError() << "writeOnBlackboard: invalid answer from blackboard: " << reply.get(0).toString();
                return false;
            }
            yInfo() << "RoomKnown written to blackboard";
        }

        {
            Bottle cmd, reply;
            cmd.clear();
            reply.clear();
            cmd.addString("set");
            cmd.addString("RobotAtRoom");
            if (flag_kitchen) cmd.addString("True");
            else cmd.addString("False");
            blackboard_port.write(cmd, reply);

            if (reply.size() != 1)
            {
                yError() << "writeOnBlackboard: invalid answer from blackboard: " << reply.toString();
                return false;
            }

            if (reply.get(0).asInt() != 1)
            {
                yError() << "writeOnBlackboard: invalid answer from blackboard: " << reply.get(0).toString();
                return false;
            }
            yInfo() << "RobotInRoom written to blackboard";
        }

        {
            Map2DLocation loc;
            bool cp = iNav->getCurrentPosition(loc);
            if (cp)
            {
                yInfo() << "Acquired current position:" << loc.toString();
            }
            else
            {
                yError() << "Unable to acquire current position";
                return false;
            }
            Bottle cmd, reply;
            cmd.clear();
            reply.clear();
            cmd.addString("set");
            cmd.addString("Room");
            string kitchen_coordinates;
            if (flag_kitchen) kitchen_coordinates = loc.map_id + " " + std::to_string(loc.x) + " " + std::to_string(loc.y) + " " + std::to_string(loc.theta);
            else              kitchen_coordinates = std::string("no_map") + " " + std::to_string(0.0) + " " + std::to_string(0.0) + " " + std::to_string(0.0);
            cmd.addString(kitchen_coordinates);
            blackboard_port.write(cmd, reply);

            if (reply.size() != 1)
            {
                yError() << "writeOnBlackboard: invalid answer from blackboard: " << reply.toString();
                return false;
            }

            if (reply.get(0).asInt() != 1)
            {
                yError() << "writeOnBlackboard: invalid answer from blackboard: " << reply.get(0).toString();
                return false;
            }
            yInfo() << "Kitchen written to blackboard";
        }

        return true;
    }

    /****************************************************************/
    bool configure(ResourceFinder &rf) override
    {
        this->setName("TextToAction");

        std::string speechTranscriptionPortName = "/"+this->getName()+"/speechTranscriptionPort:i";
        if (!speechTranscription_port.open(speechTranscriptionPortName))
        {
           yError() << this->getName() << ": Unable to open port " << speechTranscriptionPortName;
           return false;
        }

        std::string blackboardPortName= "/"+this->getName()+"/blackboard/rpc:o";
        if (!blackboard_port.open(blackboardPortName))
        {
           yError() << this->getName() << ": Unable to open port " << blackboardPortName;
           return false;
        }

        //navigation polyDriver
        Property        navCfg;
        navCfg.put("device", "navigation2DClient");
        navCfg.put("local", "/robotPathPlannerExample");
        navCfg.put("navigation_server", "/navigationServer");
        navCfg.put("map_locations_server", "/mapServer");
        navCfg.put("localization_server", "/localizationServer");
        bool ok = ddNavClient.open(navCfg);
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

        return true;
    }

    /****************************************************************/
    double getPeriod() override
    {
        return 1.0;
    }

    /****************************************************************/
    bool updateModule() override
    {
        Bottle transc_bot;
        speechTranscription_port.read(transc_bot); //blocking
        if (transc_bot.get(0).isString())
        {
            std::string transc = transc_bot.get(0).asString();
            yDebug() << "Received text: '" << transc;
            std::transform(transc.begin(), transc.end(), transc.begin(), ::tolower);
            std::transform(string1.begin(), string1.end(), string1.begin(), ::tolower);
            if (transc == string1)
            {
                yDebug() << "Text: '" << transc << "' has been recognized. Writing data on blackboard.";
                bool ret = this->writeToBlackboard(true);
                if (ret)
                {
                    yInfo() << "writeToBlackboard() successful";
                }
                else
                {
                    yError() << "writeToBlackboard() failed";
                }
            }
            else if (transc == string2)
            {
                yDebug() << "Text: '" << transc << "' has been recognized. Writing data on blackboard.";
                bool ret = this->writeToBlackboard(false);
                if (ret)
                {
                    yInfo() << "writeToBlackboard() successful";
                }
                else
                {
                    yError() << "writeToBlackboard() failed";
                }
            }
            else
            {
                yDebug() << "Text: '" << transc << "' is not recognized";
            }
        }
        return true;
    }

    /****************************************************************/
    bool respond(const Bottle &command, Bottle &reply) override
    {
        return true;
    }

    /****************************************************************/
    bool interruptModule() override
    {
        speechTranscription_port.interrupt();
        blackboard_port.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        speechTranscription_port.close();
        blackboard_port.close();

        return true;
    }
};


/****************************************************************/
int main(int argc, char *argv[])
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

    TextToAction skill;

    return skill.runModule(rf);
}
