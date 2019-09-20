/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file grasp_bottle_module.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 * @brief Behavior Tree leaf node for performing the grasping of a bottle.
 * @remarks This module requires the grasp-processor from grasp-pose-gen repository.
 */

//standard imports
#include <string>
#include <iostream>

//YARP imports
#include <yarp/os/all.h>

//behavior trees imports
#include <yarp/BT_wrappers/tick_server.h>
#include <yarp/BT_wrappers/blackboard_client.h>

#include <QApplication>

#include <QtWidgets>
#include <QPushButton>
#include <QObject>

using namespace yarp::os;
using namespace yarp::BT_wrappers;


class Roba : public QObject,
             public TickServer
{
private:
    Q_OBJECT

    ReturnStatus m_status;

public:
    QLabel * _name;
    QLabel * _target;
    QLabel * _status;
    QPushButton * SuccButt;
    QPushButton * FailButt;
    QWidget     * wid;
    Mutex       wait;

public:

    Roba(std::string actionName)
    {
        m_status = BT_IDLE;
        configure_TickServer("", actionName, true);
        // grab mutex so that it'll block in the tick callback
        wait.lock();
    }

    bool request_initialize() override
    {
        return true;
    }

    ReturnStatus request_tick(const ActionID &target, const Property &params) override
    {
        _target->setText(target.target.c_str());
        _status->setText("Running");
        _status->setStyleSheet( "background-color: yellow;"
                                "border-style: outset;"
                                "border-width: 1px;"
                                "border-color: black;");
        wait.lock();

        SuccButt->setEnabled(true);
        FailButt->setEnabled(true);
        return m_status;
    }

    ReturnStatus request_halt(const ActionID &target, const Property &params) override
    {
        return m_status;
    }

    ReturnStatus request_status(const ActionID &target) override
    {
        return m_status;
    }

    bool request_terminate() override
    {
        return true;
    }


public slots:
    void setSuccess()
    {
        std::cout << "pressed Success button for action " << _name->text().toStdString() << std::endl;
        setStuff("background-color: green;");
        m_status = BT_SUCCESS;
        wait.unlock();
    }

    void setFailure()
    {
        std::cout << "pressed Failure button for action " << _name->text().toStdString() << std::endl;
        setStuff("background-color: red;");
        m_status = BT_FAILURE;
        wait.unlock();
    }

private:
    void setStuff(std::string color)
    {
        _status->setStyleSheet(color.c_str());
        SuccButt->setEnabled(false);
        FailButt->setEnabled(false);
    }
};

#include "fake_action.moc"

Roba *newAction(std::string actionName)
{
    Roba *bella = new Roba(actionName);
    bella->wid = new QWidget;
    bella->_name = new QLabel(actionName.c_str());
    bella->_target = new QLabel("no Target");
    bella->_status = new QLabel("Status");
    bella->SuccButt = new QPushButton("Success");
    bella->FailButt = new QPushButton("Failure");

    QPainter *p;
    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(bella->_name);
    layout->addWidget(bella->_target);
    layout->addWidget(bella->_status);
    layout->addWidget(bella->SuccButt);
    layout->addWidget(bella->FailButt);
    bella->wid->setLayout(layout);

    bella->_name->setStyleSheet("border-style: outset;"
                                "border-width: 1px;"
                                "border-color: black;");

    bella->_target->setStyleSheet("border-style: outset;"
                                  "border-width: 1px;"
                                  "border-color: black;");

    bella->_status->setStyleSheet("background-color: cyan;"
                                  "border-style: outset;"
                                  "border-width: 1px;"
                                  "border-color: black;");

    QObject::connect(bella->SuccButt, SIGNAL (released()), bella, SLOT (setSuccess()));
    QObject::connect(bella->FailButt, SIGNAL (released()), bella, SLOT (setFailure()));
    return bella;
}

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;
    std::vector<Roba*> tanta;

    QApplication app(argc, argv);
    QWidgetList *l = new QWidgetList;
    QWidget *window = new QWidget;

    std::vector<std::string> actionList{"fake_fetchObject",
                                        "navigationServer",
                                        "fake_cleanFloor"
                                        };

    QVBoxLayout *layoutV = new QVBoxLayout;
    // Fill windows with action rows
    std::for_each (actionList.begin(), actionList.end(),
                    [&tanta, &layoutV](std::string name)
                    {
                        Roba* elem = newAction(name);
                        tanta.push_back(elem);
                        layoutV->addWidget(elem->wid);
                    });

    window->setLayout(layoutV);
    window->show();

    return app.exec();
}
