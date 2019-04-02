/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file main.cpp
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */

#include "mainwindow.h"
#include "monitor_reader.h"
#include "robot_interaction.h"
#include <QApplication>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <iostream>
#include <fstream>
using namespace yarp::os;
using namespace std;

int main(int argc, char *argv[])
{
    ResourceFinder rf;
    QApplication a(argc, argv);
    MainWindow w;
    MonitorReader* monitor = new MonitorReader();
    RobotInteraction* interaction = new RobotInteraction();


    if(!rf.configure(argc, argv))
    {
        yError() << "Cannot open config file " << rf.getContext() << rf.find("from").asString();
        return 1;
    }

    std::vector<std::string> names;
    // read the list of skills from a filename
    Bottle skills(rf.findGroup("skills").tail());

    for(auto i=0; i<skills.size(); i++)
    {
        names.emplace_back(skills.get(i).asList()->get(0).asString());
    }

    // configures the monitor's reader (open the YARP ports)
    bool is_ok = monitor->configure(names) && interaction->configure();

    if (!is_ok)
    {
        yError() << "[GUI] Cannot open the GUI";
        w.close();
        a.exit();
        return 1;
    }

    w.show();
    w.setupWindow(names, monitor, interaction);

    return a.exec();
}
