#####################################################
# Building examples
#####################################################

foreach(exec server_example client_example)
    message("Building ${exec} from ${exec}.cpp")
    add_executable(${exec} ${exec}.cpp)
    target_link_libraries(${exec} YARP_BT_wrappers YARP::YARP_init YARP::YARP_OS)
endforeach(exec)
