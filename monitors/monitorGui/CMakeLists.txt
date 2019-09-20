################################################################################
#                                                                              #
# Copyright (C) 2019 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################
# @authors: Michele Colledanchise <michele.colledanchise@iit.it>


find_package(Qt5 COMPONENTS Widgets)

## Enabling autogeneration of moc and ui files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(UI_SOURCES  mainwindow.ui)


set(MONTOR_SRC mainwindow.cpp
               monitor_reader.cpp
               robot_interaction.cpp
               mainwindow.ui
               mainwindow.h
    )

add_executable(monitorGui main.cpp ${MONTOR_SRC})

target_link_libraries(monitorGui ${YARP_LIBRARIES} Qt5::Widgets YARP_BT_wrappers)
