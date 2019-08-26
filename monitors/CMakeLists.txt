
set(execName "monitor")
set(MONITOR_SOURCES   src/monitor.cpp main.cpp)

add_executable(${execName} main.cpp ${MONITOR_SOURCES} )

target_include_directories(${execName} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(${execName} YARP::YARP_OS YARP::YARP_init YARP_BT_MODULES::YARP_BT_wrappers rFSMTools::rFSM)
