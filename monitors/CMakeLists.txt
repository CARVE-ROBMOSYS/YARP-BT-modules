project(carveMonitors)
cmake_minimum_required(VERSION 2.8)

find_package(rFSM QUIET)

if(rFSM_FOUND)
    include_directories(include )
    include_directories(${rFSM_INCLUDE_DIRS} )

    set(MONITOR_SOURCES   src/monitor.cpp main.cpp)

    add_executable(skill_monitor main.cpp ${MONITOR_SOURCES} ${YARP_TICK_LIB_SRC})
    target_link_libraries(skill_monitor  ${YARP_LIBRARIES} rFSMTools::rFSM)
else()
    message("rFSM not found, monitor will not be compiled")
endif()
