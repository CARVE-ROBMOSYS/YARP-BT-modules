
#########################################################
# GENERATING FILES FOR BEHAVIOR TREE'S THRIFT COMMANDS
#########################################################
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
# yarp_idl_to_dir(BTCmd.thrift ${generated_libs_dir}  sources headers include_dirs)
# yarp_idl_to_dir(BT_monitoring.thrift ${generated_libs_dir}  sources2 headers2 include_dirs2)

message("\n** Generating idl for BT_cmd ** ")
yarp_idl_to_dir(INPUT_FILES BTCmd.thrift
                OUTPUT_DIR "${PROJECT_SOURCE_DIR}/generated_files"
                SOURCES_VAR BT_CMD_SOURCES
                HEADERS_VAR BT_CMD_HEADERS
                INCLUDE_DIRS_VAR BT_CMD_INCLUDE_DIRS
#                VERBOSE
                )

message("\n** Generating idl for BT_monitor ** ")
yarp_idl_to_dir(INPUT_FILES BT_monitoring.thrift
                OUTPUT_DIR "${PROJECT_SOURCE_DIR}/generated_files"
                SOURCES_VAR BT_MON_SOURCES
                HEADERS_VAR BT_MON_HEADERS
                INCLUDE_DIRS_VAR BT_MON_INCLUDE_DIRS
#                VERBOSE
                )

include_directories(${BT_CMD_INCLUDE_DIRS} ${BT_MON_INCLUDE_DIRS} )

set(YARP_TICK_LIB_SRC   ${BT_CMD_SOURCES} ${BT_MON_SOURCES}
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


######################################################
# EXPORTS
######################################################

set(PROJECT_CONFIG ${PROJECT_NAME}Config)
export(TARGETS ${PROJECT_NAME}  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG}.cmake")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_CONFIG}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib   )

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")

install(EXPORT ${PROJECT_CONFIG}
        DESTINATION "lib/cmake")
