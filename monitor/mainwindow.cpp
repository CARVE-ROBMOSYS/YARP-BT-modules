/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)   *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file mainwindow.cpp
 * @authors: Michele Colledanchise <michele.colldanchise@iit.it>
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "monitor_reader.h"

#include <QStringList>
#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <QScrollBar>
#include <QTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    is_door_open_ = true;
    ui->setupUi(this);

}

void MainWindow::setupWindow(std::vector<std::string> name_list, MonitorReader* monitor_prt, RobotInteraction* robot_interaction_prt)
{
    QPainter painter;
    QFontMetrics fm = painter.fontMetrics();

    name_list_ = name_list;
    monitor_prt_ = monitor_prt;
    robot_interaction_prt_ = robot_interaction_prt;

    int minimum_column_0_width = 150, minimum_column_1_width = 150;
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setRowCount(name_list.size());

    // Sets the header of the table
    QStringList header;
    header <<"Skill's Name"<<"Monitor's State";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    // Sets initialization content of the table

    for (int i = 0; i< name_list_.size(); i++)
    {
        QString q_name_skill = QString(name_list_.at(i).c_str());

        int width = fm.width(q_name_skill) + 10;
        if (minimum_column_0_width < width)
        {
            minimum_column_0_width = width;
        }

        QTableWidgetItem *item = new QTableWidgetItem(q_name_skill);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(i, 0, item);

        QTableWidgetItem *wait_text = new QTableWidgetItem("Waiting for Connection");
        wait_text->setTextAlignment(Qt::AlignCenter);
        wait_text->setFlags(wait_text->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 1, wait_text);

        ui->tableWidget->item(i, 1)->setBackground(Qt::gray);

    }

    ui->tableWidget->setColumnWidth(0, minimum_column_0_width);
    ui->tableWidget->setColumnWidth(1, minimum_column_1_width);

    // sets the width of the table
    ui->tableWidget->setFixedWidth(minimum_column_0_width + minimum_column_1_width + 30 );
    ui->tableWidget->setFixedHeight((name_list.size() + 1)*ui->tableWidget->rowHeight(1) + 10);
    // Sets a timer that calls periodiacally the function update()
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(500);
}



void MainWindow::update()
{
    std::string new_message;
    bool has_message = robot_interaction_prt_->readMessage(new_message);

    if (has_message)
    {
        ui->textBrowser->append(QTime::currentTime().toString() +" The robot said: " +new_message.c_str());
    }

    // requests the updated list of states to the monitor and updates the table accordingly
    std::vector<std::string> updated_list = monitor_prt_->updateList();

    for (int i = 0; i< name_list_.size(); i++)
    {
        QString q_string_state_text = updated_list.at(i).c_str();

        if (q_string_state_text.isEmpty())
        {
            // No state read. Probably the monitor crashed
            ui->tableWidget->item(i, 1)->setBackground(Qt::gray);
            continue;
        }

        QTableWidgetItem *state_text =  new QTableWidgetItem(q_string_state_text);
        state_text->setFlags(state_text->flags() & ~Qt::ItemIsEditable);
        state_text->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 1, state_text);

        if (q_string_state_text.compare("FAILED") != 0)
        {
            ui->tableWidget->item(i, 1)->setBackground(Qt::green);
        }
        else
        {
            ui->tableWidget->item(i, 1)->setBackground(Qt::red);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_resetbb_released()
{
    robot_interaction_prt_->resetBlackboard();
}

void MainWindow::on_pushButton_this_is_released()
{
    robot_interaction_prt_->sendMessage({"this is the", "Room"});
}

void MainWindow::on_pushButton_help_released()
{
    robot_interaction_prt_->sendMessage({"help given"});

}


void MainWindow::on_pushButton_toggleDoor_released()
{
    if(is_door_open_)
    {
        robot_interaction_prt_->closeDoor();
        ui->pushButton_toggleDoor->setText("Open Door");
        is_door_open_=false;
    }
    else
    {
        robot_interaction_prt_->openDoor();
        ui->pushButton_toggleDoor->setText("Close Door");
        is_door_open_=true;
    }
}
