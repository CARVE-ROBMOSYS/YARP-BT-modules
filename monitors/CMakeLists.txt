project(carveMonitors)
cmake_minimum_required(VERSION 2.8)

find_package(rFSM )
set(execName "monitor")

if(rFSM_FOUND)
    include_directories(include $ENV{ICUBcontrib_DIR}/include )
    include_directories(${rFSM_INCLUDE_DIRS} )

    set(MONITOR_SOURCES   src/monitor.cpp main.cpp)

    add_executable(${execName} main.cpp ${MONITOR_SOURCES} ${YARP_TICK_LIB_SRC})
    target_link_libraries(${execName} ${YARP_LIBRARIES} rFSMTools::rFSM)
else()
    message("rFSM not found, monitor will not be compiled")
endif()
