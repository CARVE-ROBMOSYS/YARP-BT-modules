/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file mainwindow.h
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "monitor_reader.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setupWindow(std::vector<std::string> name_list, MonitorReader *monitor_prt);

private:
    Ui::MainWindow *ui;
    std::vector<std::string> name_list_;
    MonitorReader* monitor_prt_;

public slots:
    void update();
};

#endif // MAINWINDOW_H
