#####################################################
# Building Executables for CARVE Scenario 1
#####################################################
add_executable(blackboard_module blackboard_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(blackboard_module ${YARP_LIBRARIES} )

add_executable(meta_conditions_module meta_conditions_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(meta_conditions_module ${YARP_LIBRARIES} )

add_executable(move_joints_module move_joints_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(move_joints_module ${YARP_LIBRARIES} )

add_executable(compute_inv_pose_module compute_inv_pose_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(compute_inv_pose_module ${YARP_LIBRARIES} )

add_executable(goto_inv_pose_module goto_inv_pose_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(goto_inv_pose_module ${YARP_LIBRARIES} )

add_executable(set_inv_pose_invalid_module set_inv_pose_invalid_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(set_inv_pose_invalid_module ${YARP_LIBRARIES} )

add_executable(always_running_module always_running_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(always_running_module ${YARP_LIBRARIES} )

add_executable(locate_bottle_module locate_bottle_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(locate_bottle_module ${YARP_LIBRARIES} )

add_executable(grasp_bottle_module grasp_bottle_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(grasp_bottle_module ${YARP_LIBRARIES} )

add_executable(BTwrapper_module BTwrapper_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(BTwrapper_module ${YARP_LIBRARIES} )
