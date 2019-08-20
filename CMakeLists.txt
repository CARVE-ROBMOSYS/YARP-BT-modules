################################################################################
#                                                                              #
# Copyright (C) 2017 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################
# @authors: Michele Colledanchise <michele.colledanchise@iit.it>
# @authors: Alberto Cardellino <alberto.cardellino@iit.it>
cmake_minimum_required(VERSION 3.0)

project(YARPBTModules   LANGUAGES CXX C
                        VERSION   1.0.0)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
# FIND YCM
#########################################################
find_package(YCM REQUIRED)

#########################################################
# FIND YARP
#########################################################
find_package(YARP REQUIRED COMPONENTS OS dev math idl_tools)


#########################################################
# FIND behaviour tree library
#########################################################
find_package(BehaviorTree REQUIRED)
if(NOT BehaviorTree_FOUND)
    message(WARNING "BehaviorTree lib not found. Modules for BT engine will not be compiled.")
endif()

#########################################################
# GENERATING FILES FOR BEHAVIOR TREE'S THRIFT COMMANDS
#########################################################
# AND Building library
#####################################################
add_subdirectory(libs)

#####################################################
# Building Examples about lib
#####################################################
add_subdirectory(examples)

#####################################################
# Building executables
#####################################################
add_subdirectory(BT_engine)
add_subdirectory(modules)


include(InstallBasicPackageFiles)
install_basic_package_files(${PROJECT_NAME}
                            VERSION ${${PROJECT_NAME}_VERSION}
                            COMPATIBILITY AnyNewerVersion
                            VARS_PREFIX ${PROJECT_NAME}
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO)


# Add the uninstall target
include(AddUninstallTarget)

