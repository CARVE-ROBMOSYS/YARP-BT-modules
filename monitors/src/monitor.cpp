#include <iostream>

#include <monitor.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;


MonitorCallback::MonitorCallback(rfsm::StateMachine *sm) :
            sm(sm),
            timer(make_unique<Timer>(3.0, &MonitorCallback::callback, this, true))
{

}

MonitorCallback::MonitorCallback(rfsm::StateMachine *sm, TimerSettings& settings) :
            sm(sm),
            timer(make_unique<yarp::os::Timer>(settings, &MonitorCallback::callback, this, true))
{

}


bool MonitorCallback::callback(const yarp::os::YarpTimerEvent& timeInfo)
{
    yError() << "Timeout!! " << sm->getCurrentState();
    sm->sendEvent("e_timeout");
    sm->run();
    return true;
}

void MonitorCallback::setTimer(const TimerSettings &settings)
{
    timer->setSettings(settings);
}

void MonitorCallback::entry()
{
    timer->start();
     yInfo() << " entry callback from state " << sm->getCurrentState();
}

void MonitorCallback::doo()
{
    yInfo() << " doo callback from state " << sm->getCurrentState();
}

void MonitorCallback::exit()
{
    timer->stop();
    yInfo() << " exit callback from state " << sm->getCurrentState();
}

double Monitor::getPeriod()
{
    return 20.0;
}

bool Monitor::updateModule()
{
    // Nothing to do here, since everything runs on callback
    yDebug() << "Monitor running happily";
    return true;
}

// callback from 'debug' RPC port
bool Monitor::respond(const Bottle &command, Bottle &reply)
{
    if (command.get(0).asString() == "state")
    {
//        reply.addString(monitorSM.getCurrentState());
        return true;
    }
    reply.addString("command not recognized");

    return true;
}

// Callback from 'event' port
bool Monitor::read(ConnectionReader& connection)
{
    // Process events sent using the proper thrift message
     BTMonitorMsg monitorMsg;
     if(!monitorMsg.read(connection))
     {
         yError() << "Error reading the message";
         return false;
     }

     // process data in message
//     yInfo() << "Current state : " << monitorSM.getCurrentState();
     yInfo() << "message event : " << monitorMsg.event;
     yInfo() << "from skill    : " << monitorMsg.skill;

     if(!isEnvironment)
     {
         if(monitorMsg.skill == getName())
            monitorSM.sendEvent(monitorMsg.event);
         else
             yDebug() << "Received a monitor message from a different skill ... ignoring it";
     }
     else
     {
         // in case of monitor for environment, concatenate the event and skill name
         string eventName;
         if(monitorMsg.event == "e_from_env")
         {
             eventName = "e_success_" + monitorMsg.skill;
         }
         else
         {
             eventName = monitorMsg.event + monitorMsg.skill;
         }

         monitorSM.sendEvent(eventName);
     }
     // run means 'do all steps you have to do'
     monitorSM.run();
     yInfo() << "New state  : " << monitorSM.getCurrentState();

     return true;
}

bool Monitor::configure(yarp::os::ResourceFinder &rf)
{
    std::string filename;
    if (rf.check("filename"))
    {
        filename = rf.find("filename").asString();
    }
    else
    {
        yError() << " Missing keyword s<filename> from config file";
        return false;
    }

    if (rf.check("environment"))
    {
        isEnvironment = true;
        setName("environment");
        yInfo() << "Monitor for environment";
    }
    else
    {
        if (rf.check("name"))
        {
            setName(rf.find("name").asString().c_str());
        }
        else
        {
            yError() << " Missing keyword <name> from config file";
            return false;
        }
    }

    if (!event_port_.open("/monitor/"+getName()+"/event:i"))
    {
             yError() << "Cannot open port " << "/monitor/" << getName() << "/event:i";
        return false;
    }

    if(!state_port_.open("/monitor/"+getName()+"/rpc:i"))
    {
        yError() << "Cannot open port " << "/monitor/" << getName() << "/rpc:i";

        return false;
    }

    if(!attach(state_port_))
    {
        yError() << "Cannot attach to module the state port ";

        return false;
    }

    event_port_.setReader(*(dynamic_cast<PortReader*>(this)));

    if(!monitorSM.load(filename))
    {
        yError() << "Cannot load rFSM " << filename;
        return false;
    }

    // get the state machine description
    rfsm::StateGraph smDescription = monitorSM.getStateGraph();

    // find the events called 'e_timeout' (this name could be read from config file)

    yDebug() << rf.toString();

    Bottle eventList = rf.findGroup("timeout_events").tail();

    yDebug() << eventList.toString() << " size "  << eventList.size();

    // count number of callbacks required, in order to allocate room
    // in the vector (since rfsmtool gets references to callbacks,
    // a dynamic vector reallocations causes problems because references
    // became invalid
    size_t callbackNum = 0;

    for(auto transition : smDescription.transitions)
    {
        for(auto event : transition.events)
        {
            for(int i=0; i< eventList.size(); i++)
            {
                if(eventList.get(i).asList()->get(0).asString() == event)
                    callbackNum++;
            }
        }
    }

    yDebug() << "callbackNum " << callbackNum;
    callbacks.reserve(callbackNum);
    // ACtually fills the callback vector
    for(auto &transition : smDescription.transitions)
    {
        for(auto &event : transition.events)
        {
            for(int i=0; i< eventList.size(); i++)
            {
                string timeoutEvent = eventList.get(i).asList()->get(0).asString();
//                yDebug() << " looking for event " << timeoutEvent;
                if(timeoutEvent == event)
                {
                    callbacks.emplace_back(make_unique<MonitorCallback>(&monitorSM));
                    callbacks.rbegin()->operator ->() ->setTimer({10.0, (size_t)1, 0.0});

                    callbacks.rbegin()->operator ->()->state = transition.source;
                    monitorSM.setStateCallback(transition.source, *(*callbacks.rbegin()));
                    yInfo() << "Setting callback from state " << transition.source;

                }
                else
                {
//                    yDebug() << "NO callback for state " << transition.source << " event " << event;
                }
            }
        }
    }

    yInfo() << "Monitor for " << getName() << " ready";
    monitorSM.run();
    return true;
}

// Interrupt function.
bool Monitor::interruptModule()
{
    cout << "Interrupting your module, for port cleanup" << endl;
    return true;
}
// Close function, to perform cleanup.
bool Monitor::close()
{
    // optional, close port explicitly
    cout << "Calling close function\n";
    event_port_.close();
    return true;
}
