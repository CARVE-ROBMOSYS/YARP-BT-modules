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

using namespace yarp::os;
using namespace yarp::dev;

class moveJoint_module : public TickServer, public RFModule
{
private:
    PolyDriver  headDev;
    PolyDriver  armDev;

    bool done_headLeft  {false}, sent_headLeft  {false};
    bool done_headFront {false}, sent_headFront {false};
    bool done_arm       {false}, sent_arm       {false};
    IPositionControl  *headPos, *armPos;
    IEncoders         *headEnc, *armEnc;

    int headLeftPos{60};
public:
    ReturnStatus request_tick(const std::string& params = "") override
    {
        yInfo() << "requested tick with param " << params;
        ReturnStatus ret = BT_ERROR;
        if(params == "look_around")
        {
            if(!sent_headLeft)
            {
                sent_headLeft = true;
                headPos->positionMove(1, headLeftPos);
                ret = BT_RUNNING;
            }
            else
            {
                if(done_headLeft)
                {
                    ret = BT_SUCCESS;
                }
                else
                {
                    double enc;
                    headEnc->getEncoder(1, &enc);
                    yInfo() << "abs(enc-headLeftPos) " << abs(enc-headLeftPos);
                    if (abs(enc-headLeftPos) < 2)
                    {
                        ret = BT_SUCCESS;
                        done_headLeft = true;
                    }
                    else
                        ret = BT_RUNNING;
                }
            }
        }

        if(params == "look_front")
        {
            if(!sent_headFront)
            {
                sent_headFront = true;
                headPos->positionMove(1, 0);
                ret = BT_RUNNING;
            }
            else
            {
                if(done_headFront)
                {
                    ret = BT_SUCCESS;
                }
                else
                {
                    double enc;
                    headEnc->getEncoder(1, &enc);
                    yInfo() << "abs(enc-0) " << abs(enc-0);
                    if (abs(enc-0) < 2)
                    {
                        ret = BT_SUCCESS;
                        done_headFront = true;
                    }
                    else
                        ret = BT_RUNNING;
                }
            }
        }

        if(params == "lift_right_arm")
        {
            double poss[8] = {45.0, 20.0, -40.0, 62.0, 5.0, 0.0, 0.0, 0.0};
            ret = BT_RUNNING;

            if(!sent_arm)
            {
                sent_arm = true;
                armPos->positionMove(poss);
                ret = BT_RUNNING;
            }
            else
            {
                if(done_arm)
                {
                    ret = BT_SUCCESS;
                }
                else
                {
                    double enc[8];
                    armEnc->getEncoders(enc);

                    done_arm = true;
                    for(int i=0; i<8; i++)
                    {
                        done_arm &= (abs(enc[i] - poss[i]) < 2);
                        yInfo() << "abs(encs[i]) " << abs(enc[i] - poss[i]) << " " << done_arm;
                    }

                    if(done_arm)
                    {
                        ret = BT_SUCCESS;
                    }
                    else
                        ret = BT_RUNNING;
                }
            }
        }

        //std::this_thread::sleep_for( std::chrono::seconds(3));
        yInfo() << "requested tick with param " << params << " returning " << ret;
        return ret;
    }

    ReturnStatus request_status()
    {
        ReturnStatus ret = BT_RUNNING;
        ReturnStatusVocab a;
        yInfo() << "request_status, replying with " << a.toString(ret);
        return ret;
    }

    ReturnStatus request_halt()
    {
        ReturnStatus ret = BT_HALTED;
        ReturnStatusVocab a;
        yInfo() << "request_halt,  replying with " << a.toString(ret);
        return ret;
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

    bool configure(yarp::os::ResourceFinder &rf)
   {
       // optional, attach a port to the module
       // so that messages received from the port are redirected
       // to the respond method

        Property config;
        config.put("device", "remote_controlboard");
        config.put("remote", "/SIM_CER_ROBOT/head");
        config.put("local",  "/moveJoints/head");

        headDev.open(config);
        headDev.view(headPos);
        headDev.view(headEnc);

        config.put("remote", "/SIM_CER_ROBOT/right_arm");
        config.put("local",  "/moveJoints/right_arm");
        armDev.open(config);
        armDev.view(armPos);
        armDev.view(armEnc);

        if(! (armPos && headPos))
        {
            yError("Cannot open devices");
            return false;
        }
        return true;
   }

    bool interruptModule()
    {
        return true;
    }

    // Close function, to perform cleanup.
    bool close()
    {
        // optional, close port explicitly
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

    moveJoint_module blackboard;
    blackboard.configure_tick_server("/moveJoint");

    // initialize blackboard

    blackboard.runModule(rf);

    return 0;
}
