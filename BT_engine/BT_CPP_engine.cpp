 

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RFModule.h>

#include <yarp/os/YarpPlugin.h>

#include <behaviortree_cpp/bt_factory.h>
#include <BT_CPP_leaves/btCpp_common.h>

#include <behaviortree_cpp/blackboard.h>

// For Groot monitor
#include <behaviortree_cpp/loggers/bt_cout_logger.h>
#include <behaviortree_cpp/loggers/bt_file_logger.h>
#include <behaviortree_cpp/loggers/bt_zmq_publisher.h>
#include <behaviortree_cpp/loggers/bt_minitrace_logger.h>


using namespace BT;
using namespace std;
using namespace yarp::os;

const string plugin_path_env = "BT_CPP_PLUGIN_DIRS";

static Bottle parsePaths(const std::string& txt) {
    if (txt.empty()) {
        return Bottle();
    }
    char slash = NetworkBase::getDirectorySeparator()[0];
    char sep = NetworkBase::getPathSeparator()[0];
    Bottle result;
    const char *at = txt.c_str();
    int slash_tweak = 0;
    int len = 0;
    for (char ch : txt) {
        if (ch==sep) {
            result.addString(std::string(at, len-slash_tweak));
            at += len+1;
            len = 0;
            slash_tweak = 0;
            continue;
        }
        slash_tweak = (ch==slash && len>0)?1:0;
        len++;
    }
    if (len>0) {
        result.addString(std::string(at, len-slash_tweak));
    }
    return result;
}

class BT_Engine : public yarp::os::RFModule
{
private:
    double period;

    // We use the BehaviorTreeFactory to register our custom nodes
    Tree tree;
    BehaviorTreeFactory factory;
    std::unique_ptr<MinitraceLogger> logger_minitrace;
    std::unique_ptr<FileLogger>      logger_file;
    std::unique_ptr<StdCoutLogger>   logger_cout;
    std::unique_ptr<PublisherZMQ>    publisher_zmq;

public:

    bool configure(ResourceFinder &rf) override
    {
        this->setName("BT_engine");
        bool verbose = rf.check("verbose");

        period = rf.check("period", Value(0.020)).asDouble();


        //
        // Handle BT description xml file
        //
        // get the name of xml file describing the BT. Mandatory
        string bt_description_name = rf.find("bt_description").asString();
        if(bt_description_name == "")
        {
            yError() << "Missing <bt_description> parameter";
            return false;
        }

        // Actually find the file using the ResourceFinder
        ResourceFinder bt_description_finder;
        if(rf.check("context"))
            bt_description_finder.setDefaultContext(rf.find("context").asString().c_str());

        if(verbose)
        {
            bt_description_finder.setVerbose();
            yInfo() << "Searching for file " << bt_description_name << (rf.check("context") ? (string(" in context ") + rf.find("context").asString() + ".")
                                                                                            : string("."));
        }

        string bt_description_path =  bt_description_finder.findFileByName(bt_description_name);
        if(bt_description_path == "")
        {
            yError() << string("Can't find <") + bt_description_name + "> file.";
            return false;
        }
        yDebug() << bt_description_path;

        //
        // Handle list of node libraries
        //
        // get the list of shared libraries containing the node classes
        Value input = rf.find("libraries");
        Bottle *libraries_names;
        if(input.isList())
            libraries_names = input.asList();
        else
            libraries_names = new Bottle;

        // include default libs, always shipped with this executable
        libraries_names->addString("libBT_CPP_leaves.so");

        // Read env variables with path into which search for libraries
        Bottle paths = parsePaths(NetworkBase::getEnvironment(plugin_path_env.c_str()));
        if(verbose && paths.size() == 0)
        {
            yWarning() << "Environment variable " << plugin_path_env << " is missing, BT plugins may not be found";
        }

        if(verbose)
            yDebug() << "Looking for node libraries " << libraries_names->toString();


        char slash = NetworkBase::getDirectorySeparator()[0];

        // Keep track of libraries already found ... loading two times the same one creates issues
        std::map<string, bool> libFound;

        // Search all required libraries
        for(int lib=0; lib<libraries_names->size(); lib++)
        {
            string libName = libraries_names->get(lib).toString();
            auto it = libFound.find( libName);
            if (it != libFound.end())
            {
                // lib already found
                continue;
            }

            bool found = false;
            // search in all known paths
            for(int path=0; path<paths.size(); path++)
            {
                try {
                    string libPath = paths.get(path).asString() + slash + libName;
                    yInfo() << "looking for " << libPath;
                    factory.registerFromPlugin(libPath);
                    found = true;
                    break;
                }
                catch(RuntimeError err)
                {
                    // lib not found
                    yError() << err.what();
                }
            }

            libFound[libName] = found;
            if(!found)
            {
                yError() << "Cannot find library " << libraries_names->get(lib).toString() << " inside provided paths " << paths.toString() <<
                            "\n\tPlease update environment variable " << plugin_path_env << " with the path containing the library.";
                return false;
            }
        }

        if(!input.isList())
            delete libraries_names;

        //
        // find parameter file, if any
        //
        Property params;

        // Actually find the file using the ResourceFinder
        ResourceFinder params_file_finder;
        string param_file_name= "";
        if(rf.check("params_file"))
        {
            params_file_finder.setDefaultContext(rf.find("context").asString().c_str());
            param_file_name = rf.find("params_file").asString();

            if(verbose)
            {
                params_file_finder.setVerbose();
                yInfo() << "Searching for parameters file " << param_file_name << (rf.check("context") ? (string(" in context ") + rf.find("context").asString() + ".")
                                                                                                : string("."));
            }

            string param_file_path =  params_file_finder.findFileByName(param_file_name);
            if(param_file_path == "")
            {
                yError() << string("Can't find <") + param_file_name + "> file.";
                return false;
            }
            yDebug() << param_file_path;
            yDebug() << params_file_finder.toString();

            // Fill in a Propety
            params.fromConfigFile(param_file_path);
            yDebug() << params.toString();
        }

        // Trees are created at deployment-time (i.e. at run-time, but only once at the beginning).
        // The currently supported format is XML.
        // IMPORTANT: when the object "tree" goes out of scope, all the TreeNodes are destroyed

        yInfo() << "Loading BT from file" << bt_description_path;
        tree = factory.createTreeFromFile(bt_description_path);

        //
        // Make sure to call 'init' function on each node, if present
        //

        bool ret{true};
        yInfo() << "tree.nodes size is " << tree.nodes.size();
        for( auto& node: tree.nodes )
        {
            yInfo() << "Initting " << node->name();

            if( auto action_B_node = dynamic_cast<bt_cpp_modules::iBT_CPP_modules*>( node.get() ))
            {
                yInfo() << "Calling init for node " << node->name();
                if(! action_B_node->initialize(params) )
                {
                    ret = false;
                    yError() << node->name() << "failed to initialize. Cannot start the BT engine.";
                }
            }
        }
        if(!ret)
        {
            yError() << "Some nodes failed to initialize. Quitting.";
            return false;
        }

        // Open ZMQ socket for Groot GUI

        logger_minitrace = make_unique<MinitraceLogger>(tree, "bt_trace.json");
        logger_file = make_unique<FileLogger>    (tree, "bt_trace.fbl");
        logger_cout = make_unique<StdCoutLogger> (tree);
        publisher_zmq = make_unique<PublisherZMQ>(tree);

        printTreeRecursively(tree.root_node);
//        std::this_thread::sleep_for(std::chrono::seconds(2));

        for(int i : {1,0})
        {
            NodeStatus status = NodeStatus(i);
            for(auto node : tree.nodes)
            {
                node->setStatus((NodeStatus)i);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        std::cout << "\n\nInitialization succesfull ...\n" << std::endl;
        return true;
    }

    /****************************************************************/
    double getPeriod() override
    {
        return period;
    }

    /****************************************************************/
    bool updateModule() override
    {
        // To "execute" a Tree you need to "tick" it.
        // The tick is propagated to the children based on the logic of the tree.
        // In this case, the entire sequence is executed, because all the children
        // of the Sequence return SUCCESS.

        int i{0};
        yDebug() << "start running the BT ";

        std::cout << "\nIteration num " << i++ << "\n";
        tree.root_node->executeTick();

        return true;
    }

    /****************************************************************/
    bool respond(const Bottle &command, Bottle &reply) override
    {

    }

    /****************************************************************/
    bool interruptModule() override
    {
        yDebug() << "Ended with " << toStr(tree.root_node->status());
        for( auto& node: tree.nodes )
        {
            yInfo() << "Terminating nodes " << node->name();

            if( auto action_B_node = dynamic_cast<bt_cpp_modules::iBT_CPP_modules*>( node.get() ))
            {
                yInfo() << "Calling terminate for node " << node->name();
                if(! action_B_node->terminate() )
                {
                    yError() << node->name() << "Terminate action for node " << node->name() << "ended with error";
                }
            }
        }
        return true;
    }

    /****************************************************************/
    bool close() override
    {
        yTrace();
        return true;
    }
};

int main(int argc, char *argv[])
{
    yarp::os::Network init;

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    BT_Engine engine;
    return engine.runModule(rf);
}
