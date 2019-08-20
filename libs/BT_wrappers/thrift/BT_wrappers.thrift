# BTCmd.thrift
#
/**
 * Enumerates the states every node can be in after execution during a particular
 * time step:
 * - "BT_SUCCESS" indicates that the node has completed running during this time step;
 * - "BT_FAILURE" indicates that the node has determined it will not be able to complete
 *   its task;
 *
 * The following  returns statuses are not used in the thrift. But used in the Behavior
 * tree core. To make this project independend I just copied the enum from the core
 * (yes if someone changes the enum it could be dangerous).
 *
 * The statuses are:
 *
 * - "BT_RUNNING" indicates that the node has successfully moved forward during this
 *   time step, but the task is not yet complete;
 * - "BT_IDLE" indicates that the node hasn't run yet.
 * - "BT_HALTED" indicates that the node has been halted by its parent.
 * - "BT_ERROR" indicates the something wrong happened.
 */
namespace yarp yarp.BT_wrappers

struct Params { }
(
  yarp.name = "yarp::os::Property"
  yarp.includefile="yarp/os/Property.h"
)

enum ReturnStatus {BT_IDLE, BT_RUNNING, BT_SUCCESS, BT_FAILURE, BT_HALTED, BT_ERROR}

struct ActionID {
    1: string target;
    2: string resources;
    3: i32 action_ID
    4: string BT_filename
}

# chimate RPC con timeout?
service BT_request {
    ReturnStatus request_status(1:ActionID target);
    ReturnStatus request_tick  (1:ActionID target, 2: Params params = {});
    ReturnStatus request_halt  (1:ActionID target, 2: Params params = {});
    bool request_initialize();
    bool request_terminate ();
}
