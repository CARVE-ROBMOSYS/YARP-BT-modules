


#####################################################
# Building Executables for CARVE Scenario
#####################################################
add_executable(blackboard_module blackboard_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(blackboard_module ${YARP_LIBRARIES} )

add_executable(meta_conditions_module meta_conditions_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(meta_conditions_module ${YARP_LIBRARIES} )

add_executable(move_joints_module move_joints_module.cpp ${YARP_TICK_LIB_SRC})
target_link_libraries(move_joints_module ${YARP_LIBRARIES} )
