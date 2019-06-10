################################################################################
#                                                                              #
# Copyright (C) 2017 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################
# @authors: Michele Colledanchise <michele.colledanchise@iit.it>

project(YARPBTModules)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

##########################################################
## ENABLE C++14 and thread
##########################################################

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

#########################################################
# FIND YARP
#########################################################
find_package(YARP REQUIRED)

#########################################################
# GENERATING FILES FOR BEHAVIOR TREE'S THRIFT COMMANDS
#########################################################
# AND Building library
#####################################################

add_subdirectory(thrift)


#####################################################
# Building Examples about lib
#####################################################

add_executable(client_example examples/client_example.cpp)
target_link_libraries(client_example ${PROJECT_NAME} )

add_executable(server_example examples/server_example.cpp)
target_link_libraries(server_example ${PROJECT_NAME})

add_subdirectory(scenarios)
add_subdirectory(monitor)
add_subdirectory(monitors)


