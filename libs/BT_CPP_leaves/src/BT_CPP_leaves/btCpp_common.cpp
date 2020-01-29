
/******************************************************************************
*                                                                            *
* Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
* All Rights Reserved.                                                       *
*                                                                            *
******************************************************************************/
/**
 * @file yarp_bt_modules.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 *           Alberto Cardellino <alberto.cardellino@iit.it>
 */

#include "btCpp_client.h"
#include "btCpp_server.h"
#include "btCpp_common.h"
#include "btCpp_checkCondition.h"
#include "btCpp_setCondition.h"
#include "btCpp_checkRobotInRoom.h"
#include "btCpp_checkRobotAtLocation.h"

#include <behaviortree_cpp_v3/bt_factory.h>


// Register at once all the Actions and Conditions in this file
BT_REGISTER_NODES(factory)
{
    // modules with generic name
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("YARP_tick_client");
    factory.registerNodeType<bt_cpp_modules::BtCpp_DummyServer>("YARP_tick_server");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("YARP_check_condition");
    factory.registerNodeType<bt_cpp_modules::BtCppSetCondition>("YARP_set_condition");

    // modules with specific names
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("BtClient");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("GoToRoom_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("GoToLocation_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("Follower_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("FindObject_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("LocateObject_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("ComputePose_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("GraspObject_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("KeepArmsForGrasp_cl");
    factory.registerNodeType<bt_cpp_modules::BtCppClient>("AskForHelp_cl");

    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("CheckCondition");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("LocationKnown");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("RobotInRoom");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("RobotAtLocation");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("InvPoseComputed");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("InvPoseValid");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("ObjectGrasped");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("ObjectLocated");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckCondition>("Set_InvPoseComputed");

    // Perception for navigation
    factory.registerNodeType<bt_cpp_modules::BtCppCheckRootInRoom>("IsRobotInRoom");
    factory.registerNodeType<bt_cpp_modules::BtCppCheckRobotAtLocation>("IsRobotAtLocation");

    factory.registerNodeType<bt_cpp_modules::BtCppSetCondition>("SetCondition");
    factory.registerNodeType<bt_cpp_modules::BtCppResetCondition>("ResetCondition");
    factory.registerNodeType<bt_cpp_modules::BtCppResetCondition>("ResetInvPoseValid");
    factory.registerNodeType<bt_cpp_modules::BtCppResetCondition>("ResetInvPoseComputed");

}
