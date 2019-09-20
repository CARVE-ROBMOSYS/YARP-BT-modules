
#########################################################
# GENERATING FILES FOR BEHAVIOR TREE'S THRIFT COMMANDS
#########################################################
set(BT_WRAP_LIB_NAME YARP_BT_wrappers)

message("\n** Generating idl for BT_module ** ")

yarp_idl_to_dir(INPUT_FILES     "${CMAKE_CURRENT_SOURCE_DIR}/thrift/BT_wrappers.thrift"
                                "${CMAKE_CURRENT_SOURCE_DIR}/thrift/BlackBoard.thrift"
                                "${CMAKE_CURRENT_SOURCE_DIR}/thrift/BT_monitoring.thrift"
                OUTPUT_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/generated_files"
                SOURCES_VAR      BT_WRAP_SOURCES
                HEADERS_VAR      BT_WRAP_HEADERS
                INCLUDE_DIRS_VAR BT_WRAP_INCLUDE_DIRS
                )

set(YARP_WRAP_LIB_SRCS  ${BT_WRAP_SOURCES} ${BT_MON_SOURCES}
                        src/yarp/BT_wrappers/tick_server.cpp
                        src/yarp/BT_wrappers/tick_client.cpp
                        src/yarp/BT_wrappers/blackboard_client.cpp)

set(YARP_WRAP_LIB_HDRS  ${BT_WRAP_HEADERS}
                        ${BT_MON_HEADERS}
                        src/yarp/BT_wrappers/tick_server.h
                        src/yarp/BT_wrappers/tick_client.h
                        src/yarp/BT_wrappers/blackboard_client.h)


#####################################################
# Building library
#####################################################

add_library(${BT_WRAP_LIB_NAME} SHARED ${YARP_WRAP_LIB_SRCS} ${YARP_WRAP_LIB_HDRS})
add_library(YARP_BT_MODULES::${BT_WRAP_LIB_NAME} ALIAS ${BT_WRAP_LIB_NAME})

target_compile_features(${BT_WRAP_LIB_NAME} PRIVATE   cxx_std_11
                                            INTERFACE cxx_std_11)

target_include_directories(${BT_WRAP_LIB_NAME}
                            PUBLIC      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>   # include folder, inside build
                                        $<BUILD_INTERFACE:${BT_WRAP_INCLUDE_DIRS}>
                                        $<BUILD_INTERFACE:${BT_MON_INCLUDE_DIRS}>

                                        $<INSTALL_INTERFACE:include>                              # include folder, after installation
                            )

target_link_libraries(${BT_WRAP_LIB_NAME} PUBLIC YARP::YARP_OS)

# list headers file to be installed
set_property( TARGET    ${BT_WRAP_LIB_NAME}
              PROPERTY  PUBLIC_HEADER   ${YARP_WRAP_LIB_HDRS})


######################################################
# EXPORTS
######################################################

install(TARGETS   ${BT_WRAP_LIB_NAME}
        EXPORT    ${PROJECT_NAME}
        COMPONENT ${BT_WRAP_LIB_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${BT_WRAP_LIB_NAME})



# creation of config file is done once in the main CMakeLists.
