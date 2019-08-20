
add_subdirectory(BT_wrappers)

if(BehaviorTree_FOUND)
    add_subdirectory(BT_CPP_leaves)
endif(BehaviorTree_FOUND)
