################################################################################
#                                                                              #
# Copyright (C) 2019 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################
# @authors: Michele Colledanchise <michele.colledanchise@iit.it>
#           Alberto Cardellino <alberto.cardellino@iit.it>

add_executable(BT_CPP_engine  BT_CPP_engine.cpp )
# target_compile_definitions(BT_engine_cpp PRIVATE "MANUAL_STATIC_LINKING")
target_link_libraries(BT_CPP_engine YARP::YARP_init YARP::YARP_OS BehaviorTree::behaviortree_cpp_v3 BT_CPP_leaves)

install(TARGETS BT_CPP_engine DESTINATION bin)
