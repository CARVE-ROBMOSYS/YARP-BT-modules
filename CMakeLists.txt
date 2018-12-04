################################################################################
#                                                                              #
# Copyright (C) 2017 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################
# @authors: Michele Colledanchise <michele.colledanchise@iit.it>

project(YARPBTModules)
cmake_minimum_required(VERSION 2.8)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

##########################################################
## ENABLE C++11 and thread
##########################################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

##########################################################
## ENABLE C++11
##########################################################
#include(CheckCXXCompilerFlag)
#unset(CXX11_FLAGS)
#check_cxx_compiler_flag("-std=c++11" CXX_HAS_STD_CXX11)
#if(CXX_HAS_STD_CXX11)
#  set(CXX11_FLAGS "-std=c++11")
#endif()

#########################################################
# FIND YARP
#########################################################
find_package(YARP REQUIRED)

#########################################################
# GENERATING FILES FOR BEHAVIOR TREE'S THRIFT COMMANDS
#########################################################
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
yarp_idl_to_dir(BTCmd.thrift ${generated_libs_dir}  sources headers include_dirs)

message("sources ${sources}")

include_directories(${CMAKE_SOURCE_DIR} )
include_directories(${PROJECT_SOURCE_DIR}/include/)


# set(YARP_BT_MODULES_SRC src/yarp_bt_module.cpp
#                         src/BTCmd.cpp
#                         src/ReturnStatus.cpp)

set(YARP_TICK_LIB_SRC   ${sources}
                        ${PROJECT_SOURCE_DIR}/src/tick_server.cpp
                        ${PROJECT_SOURCE_DIR}/src/tick_client.cpp
                        )


#####################################################
# Building library
#####################################################

add_library(YARP_tickable SHARED ${YARP_TICK_LIB_SRC})
target_link_libraries(YARP_tickable ${YARP_LIBRARIES})


#####################################################
# Building Executables
#####################################################
# add_executable(action_example_module examples/action_example_module.cpp  ${YARP_BT_MODULES_SRC})
# target_link_libraries(action_example_module ${YARP_LIBRARIES} )
#
# add_executable(condition_example_module examples/condition_example_module.cpp ${YARP_BT_MODULES_SRC})
# target_link_libraries(condition_example_module ${YARP_LIBRARIES} )

#####################################################
# Building Examples about lib
#####################################################

add_executable(client_example examples/client_example.cpp  ${YARP_TICK_LIB_SRC})
target_link_libraries(client_example ${YARP_LIBRARIES} )

add_executable(server_example examples/server_example.cpp  ${YARP_TICK_LIB_SRC})
target_link_libraries(server_example ${YARP_LIBRARIES} )

add_subdirectory(scenarios)





