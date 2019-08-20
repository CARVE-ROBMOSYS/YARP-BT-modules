
###########################################################
# Library for integration with behaviour_CPP based engine
###########################################################
set(BT_CPP_LIBRARY BT_CPP_leaves)

set(BT_CPP_LIB_SRCS     src/BT_CPP_leaves/btCpp_server.cpp
                        src/BT_CPP_leaves/btCpp_client.cpp
                        src/BT_CPP_leaves/btCpp_common.cpp
                        src/BT_CPP_leaves/btCpp_checkCondition.cpp
                        src/BT_CPP_leaves/btCpp_setCondition.cpp
                        src/BT_CPP_leaves/btCpp_checkRobotAtLocation.cpp
                        src/BT_CPP_leaves/btCpp_checkRobotInRoom.cpp
                        )

set(BT_CPP_LIB_HDRS     src/BT_CPP_leaves/btCpp_server.h
                        src/BT_CPP_leaves/btCpp_client.h
                        src/BT_CPP_leaves/btCpp_common.h
                        src/BT_CPP_leaves/btCpp_checkCondition.h
                        src/BT_CPP_leaves/btCpp_setCondition.h
                        src/BT_CPP_leaves/btCpp_checkRobotAtLocation.h
                        src/BT_CPP_leaves/btCpp_checkRobotInRoom.h
                        )

#####################################################
# Building library
#####################################################

add_library(${BT_CPP_LIBRARY}  SHARED   ${BT_CPP_LIB_SRCS} ${BT_CPP_LIB_HDRS})
add_library(YARP_BT_MODULES::${BT_CPP_LIBRARY} ALIAS ${BT_CPP_LIBRARY})

target_link_libraries(${BT_CPP_LIBRARY} PUBLIC  YARP::YARP_init YARP::YARP_OS YARP::YARP_dev YARP::YARP_math
                                                YARP_BT_wrappers BehaviorTree::behaviortree_cpp_v3)

message("BT_PLUGIN_EXPORT is ${BT_PLUGIN_EXPORT}")
target_compile_definitions(${BT_CPP_LIBRARY} PRIVATE  BT_PLUGIN_EXPORT )
#set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

target_include_directories(${BT_CPP_LIBRARY}
                            PUBLIC      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>    # include folder, inside build
                                        $<INSTALL_INTERFACE:include>                           # include folder, after installation
                            )

# list headers file to be installed
set_property( TARGET    ${BT_CPP_LIBRARY}
              PROPERTY    PUBLIC_HEADER   ${BT_CPP_LIB_HDRS})

######################################################
# EXPORTS
######################################################


message("LIB_NAME is ${BT_CPP_LIBRARY} ")
install(TARGETS   ${BT_CPP_LIBRARY}
        EXPORT    ${PROJECT_NAME}
        COMPONENT ${BT_CPP_LIBRARY}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${BT_CPP_LIBRARY})



# creation of config file is done once in the main CMakeLists.
