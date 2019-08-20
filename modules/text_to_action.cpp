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
#include <iostream>

//YARP imports
#include <yarp/os/all.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/sig/all.h>

#include <yarp/BT_wrappers/blackboard_client.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::BT_wrappers;

/****************************************************************/
class TextToAction : public RFModule
{
    private:
    Port      speechTranscription_port;         // should be connected to ...
    BlackBoardClient blackboardClient;

    std::string identifyCmd;
    std::string forgetCmd;
    std::string object;
    PolyDriver ddNavClient;
    INavigation2D* iNav;
    bool setLocation{false};                    // use localization server to store position of objects

    public:
    TextToAction ()
    {
        identifyCmd = "this is the";
        forgetCmd = "forget the";
        iNav = 0;
    }

    bool writeToBlackboard(bool identified, string &what)
    {
        {
            Property data;
            if (identified)
                data.put("known", true);
            else
                data.put("known", false);

            blackboardClient.setData(what, data);
            yInfo() << what << " written to blackboard";
        }

        if(setLocation)
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

            Property p;
            Property &loc_p = p.addGroup("location");
            loc_p.put("map_id", loc.map_id);
            loc_p.put("x", loc.x);
            loc_p.put("y", loc.y);
            loc_p.put("theta", loc.theta);

            p.put("robot_radius", 0.35f);
            p.put("obstacle_avoidance", true);
            p.put("room", true);

            blackboardClient.setData(what, p);
            yInfo() << what << "data written to blackboard";
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

        blackboardClient.configureBlackBoardClient("", getName());
        blackboardClient.connectToBlackBoard();


        if(rf.check("noLocation"))
            setLocation = false;

        if(setLocation)
        {
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
            int i=0;
            std::string transc = transc_bot.get(0).asString();
            for(i=1; i< transc_bot.size() -1; i++)
            {
                transc += " " + transc_bot.get(i).asString();
            }
            object = transc_bot.get(i).asString();
            std::cout << "Received text: '" << transc << " <" << object << ">" << std::endl;
            std::transform(transc.begin(), transc.end(), transc.begin(), ::tolower);

            if (transc == identifyCmd)
            {
                std::cout << "Text: '" << transc << " <" << object << ">' has been recognized. Writing data on blackboard." << std::endl;
                bool ret = this->writeToBlackboard(true, object);
                if (ret)
                {
                    yInfo() << "writeToBlackboard() successful";
                }
                else
                {
                    yError() << "writeToBlackboard() failed";
                }
            }
            else if (transc == forgetCmd)
            {
                std::cout << "Text: '" << transc << " <" << object << ">' has been recognized. Writing data on blackboard." << std::endl;
                bool ret = this->writeToBlackboard(false, object);
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
        return true;
    }

    /****************************************************************/
    bool close() override
    {
        speechTranscription_port.close();
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
