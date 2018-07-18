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


#########################################################
# ENABLE C++11
#########################################################
include(CheckCXXCompilerFlag)
unset(CXX11_FLAGS)
check_cxx_compiler_flag("-std=c++11" CXX_HAS_STD_CXX11)
if(CXX_HAS_STD_CXX11)
  set(CXX11_FLAGS "-std=c++11")
endif()

#########################################################
# FIND YARP
#########################################################
find_package(YARP REQUIRED)


# pick up YARP find scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})


#########################################################
# GENERATING FILES FOR BEHAVIOR TREE'S THRIFT COMMANDS
#########################################################
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
yarp_idl_to_dir(BTCmd.thrift ${generated_libs_dir}
    sources headers include_dirs)


include_directories(${CMAKE_SOURCE_DIR}
                    ${YARP_INCLUDE_DIRS}
                    )

include_directories(${PROJECT_SOURCE_DIR}/include/)



set(YARP_BT_MODULES_SRC
${PROJECT_SOURCE_DIR}/src/yarp_bt_module.cpp
${PROJECT_SOURCE_DIR}/src/BTCmd.cpp)


#####################################################
#Building Executables
#####################################################
add_executable(action_example_module examples/action_example_module.cpp  ${YARP_BT_MODULES_SRC})
target_link_libraries(action_example_module ${YARP_LIBRARIES} )


add_executable(condition_example_module examples/condition_example_module.cpp ${YARP_BT_MODULES_SRC})
target_link_libraries(condition_example_module ${YARP_LIBRARIES} )




