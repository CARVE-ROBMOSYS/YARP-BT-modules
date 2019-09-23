#####################################################
# Building Executables for CARVE Scenario 1
#####################################################

find_package(Qt5 COMPONENTS Widgets Qml Quick QuickWidgets REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(examples    fake_action)

# build and install
foreach(exec ${examples})
    add_executable(${exec} ${exec}.cpp)
    target_link_libraries(${exec}   YARP_BT_wrappers YARP::YARP_init YARP::YARP_OS
                                    Qt5::Widgets  Qt5::Qml Qt5::Quick Qt5::QuickWidgets)

    install(TARGETS ${exec} DESTINATION bin)
endforeach(exec)

