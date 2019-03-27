#ifndef MONITOR_H
#define MONITOR_H

#include <memory>

#include <yarp/os/Port.h>
#include <yarp/os/Timer.h>
#include <yarp/os/Timer.h>

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/PortablePair.h>

#include <rfsm.h>
#include <BTMonitorMsg.h>

class MonitorCallback : public rfsm::StateCallback
{
public:
    MonitorCallback(rfsm::StateMachine *sm);
    MonitorCallback(rfsm::StateMachine *sm, yarp::os::TimerSettings& settings);

    MonitorCallback (MonitorCallback && other)
    {
        std::cout << "Move ctr";
        sm = other.sm;
        timer = std::move(other.timer);
        state = other.state;
    }

    MonitorCallback & operator = (MonitorCallback && other)
    {
        std::cout << "Move operator";
        sm = other.sm;
        timer = std::move(other.timer);
        state = other.state;
    }

    void entry();
    void doo();
    void exit();

    bool callback(const yarp::os::YarpTimerEvent& timeInfo);
    void setTimer(const yarp::os::TimerSettings & settings);

    std::string state;
private:
    rfsm::StateMachine *sm;
    std::unique_ptr<yarp::os::Timer> timer;
};

class Monitor : public yarp::os::RFModule,
                public yarp::os::PortReader
{
public:
    bool configure(yarp::os::ResourceFinder &rf);

private:
    double getPeriod();
    bool updateModule();
    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
    bool interruptModule();
    bool close();

    // callback for event port
    bool read(yarp::os::ConnectionReader& connection);

    bool isEnvironment {false};

    yarp::os::Port event_port_; // a port to handle event messages
    yarp::os::Port state_port_; // a port to rpc requests
    rfsm::StateMachine monitorSM;
    std::vector<std::unique_ptr<MonitorCallback> > callbacks;
};

#endif // MONITOR_H
