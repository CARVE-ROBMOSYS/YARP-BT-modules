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


    rf.configure(argc, argv);


    std::string filename = rf.find("filename").asString();
    std::vector<std::string> names;

    // read the list of skills from a filename
    ifstream input(filename);
    if (input.is_open())
    {
        std::string name;

        while (input >> name)
        {
            names.push_back(name);
        }
    }
    else
    {
        yError() << "Cannot open the file" << filename;
        return 1;
    }

    // configures the monitor's reader (open the YARP ports)
    bool is_ok = monitor->configure(names);

    if (!is_ok)
    {
        yError() << "Cannot open the monitor GUI";
        return 1;
    }

    w.show();
    w.setupWindow(names, monitor);

    return a.exec();
}
