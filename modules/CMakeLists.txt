#####################################################
# Building Executables for CARVE Scenario 1
#####################################################

set(modules     ask_for_help
                blackboard_module
                compute_inv_pose_module
                grasp_object_module
                locate_object_module
                move_joints_module
                navigation_module
                pour_drink_module
                set_inv_pose_invalid_module
                text_to_action
    )


# build and install
foreach(exec ${modules})
    add_executable(${exec} ${exec}.cpp)
    target_link_libraries(${exec} YARP_BT_wrappers YARP::YARP_init YARP::YARP_OS YARP::YARP_sig YARP::YARP_dev YARP::YARP_math)
    install(TARGETS ${exec} DESTINATION bin)
endforeach(exec)

add_subdirectory(examples)
