


#####################################################
# Building Executables for CARVE Scenario
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
