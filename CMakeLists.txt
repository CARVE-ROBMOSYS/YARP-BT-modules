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
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
yarp_idl_to_dir(BTCmd.thrift ${generated_libs_dir}  sources headers include_dirs)
yarp_idl_to_dir(BT_monitoring.thrift ${generated_libs_dir}  sources2 headers2 include_dirs2)

message("sources ${sources}")
message("sources2 ${sources2}")

include_directories(${CMAKE_SOURCE_DIR} )
include_directories(${PROJECT_SOURCE_DIR}/include/)


# set(YARP_BT_MODULES_SRC src/yarp_bt_module.cpp
#                         src/BTCmd.cpp
#                         src/ReturnStatus.cpp)

set(YARP_TICK_LIB_SRC   ${sources} ${sources2}
                        ${PROJECT_SOURCE_DIR}/src/tick_server.cpp
                        ${PROJECT_SOURCE_DIR}/src/tick_client.cpp
                        )


#####################################################
# Building library
#####################################################

add_library(${PROJECT_NAME} SHARED ${YARP_TICK_LIB_SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC ${YARP_LIBRARIES})

target_include_directories(${PROJECT_NAME}
        PUBLIC      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:include>)


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


######################################################
# EXPORTS

set(PROJECT_CONFIG ${PROJECT_NAME}Config)
export(TARGETS ${PROJECT_NAME}
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}.cmake")

install(TARGETS ${PROJECT_NAME}
   EXPORT ${PROJECT_CONFIG}
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib   )

INSTALL( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")

install(EXPORT ${PROJECT_CONFIG}
    DESTINATION "lib/cmake")
