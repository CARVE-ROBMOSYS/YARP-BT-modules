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
#include "robot_interaction.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setupWindow(std::vector<std::string> name_list, MonitorReader *monitor_prt, RobotInteraction* robot_interaction_prt_);

private:
    Ui::MainWindow *ui;
    std::vector<std::string> name_list_;
    MonitorReader* monitor_prt_;
    RobotInteraction* robot_interaction_prt_;
    bool is_door_open_;


public slots:
    void update();
private slots:
    void on_pushButton_resetbb_released();
    void on_pushButton_this_is_released();
    void on_pushButton_help_released();
    void on_pushButton_toggleDoor_released();
};

#endif // MAINWINDOW_H
