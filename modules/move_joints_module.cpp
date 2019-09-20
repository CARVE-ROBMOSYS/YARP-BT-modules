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
#include <chrono>
#include <numeric>
#include <iostream>

//YARP imports
#include <yarp/os/Network.h>        // for yarp::os::Network
#include <yarp/os/LogStream.h>      // for yError()
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IEncoders.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::BT_wrappers;

class moveJoint_module : public TickServer, public RFModule
{
private:
    PolyDriver  headDev;
    PolyDriver  armDev;

    string robotPort_prefix{"/cer"};
    bool done_headLeft  {false}, sent_headLeft  {false};
    bool done_headFront {false}, sent_headFront {false};
    bool done_arm       {false}, sent_arm       {false};
    IPositionControl  *headPos, *leftArmPos, *rightArmPos;
    IEncoders         *headEnc, *leftArmEnc, *rightArmEnc;

    int headLeftPos{60};
public:

    ReturnStatus request_tick(const ActionID &target, const yarp::os::Property &params = {}) override
    {
        yInfo() << "requested tick with param " << params.toString();
        ReturnStatus ret = BT_ERROR;

        if(!params.check("type"))
        {
            yError() << "Node <" + this->getName() + "> missing parameter <type>";
            return BT_ERROR;
        }

        std::string type = params.find("type").asString();

        if(type == "look_around")
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

        if(type == "look_front")
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

        if(type == "homeArms")
        {
            // TODO: read from blackboard
            int nJoints = 5;
            std::vector<int> joints(nJoints);
            std::vector<double> armTargets{45, 60, 10, 80, 18};
            std::iota(begin(joints), end(joints), 0);

            leftArmPos ->positionMove(nJoints, joints.data(), armTargets.data());
            rightArmPos->positionMove(nJoints, joints.data(), armTargets.data());

            ret = BT_RUNNING;

            double enc[8];
            leftArmEnc->getEncoders(enc);

            // TBD: there should be a instance of 'done_arm' flag for each target!! Place it in the map!
            while(!done_arm && (!isHaltRequested(target)))
            {
                done_arm = true;
                for(int i=0; i<8; i++)
                {
                    done_arm &= (abs(enc[i] - armTargets[i]) < 2);
                    yInfo() << "abs(encs[i]) " << abs(enc[i] - armTargets[i]) << " " << done_arm;
                }

                done_arm ?  ret = BT_SUCCESS :  ret = BT_RUNNING;
                std::this_thread::sleep_for( std::chrono::milliseconds(200));
            }
        }

        yInfo() << "requested tick with param " << params.toString() << " returning " << ret;
        return (isHaltRequested(target) ? BT_HALTED : ret);
    }

    ReturnStatus request_status(const ActionID &target) override
    {
        ReturnStatus ret = BT_RUNNING;
        ReturnStatusVocab a;
        yInfo() << "request_status, replying with " << a.toString(ret);
        return ret;
    }

    ReturnStatus request_halt(const ActionID &target, const yarp::os::Property &params = {}) override
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

        robotPort_prefix = rf.check("robot", Value("/cer")).asString();
        Property config;
        config.put("device", "remote_controlboard");
        config.put("remote", robotPort_prefix+"/head");
        config.put("local",  "/moveJoints/head");

        headDev.open(config);
        headDev.view(headPos);
        headDev.view(headEnc);

        // open client for left arm
        config.put("remote", robotPort_prefix+"/left_arm");
        config.put("local",  "/moveJoints/left_arm");
        armDev.open(config);
        armDev.view(leftArmPos);
        armDev.view(leftArmEnc);

        // open client for right arm
        config.put("remote", robotPort_prefix+"/right_arm");
        config.put("local",  "/moveJoints/right_arm");
        armDev.open(config);
        armDev.view(rightArmPos);
        armDev.view(rightArmEnc);

        if(! (leftArmPos && rightArmPos && headPos))
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
    blackboard.configure_TickServer("", "moveJoints");

    // initialize blackboard

    blackboard.runModule(rf);

    return 0;
}
