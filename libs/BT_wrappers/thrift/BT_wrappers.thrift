
namespace yarp yarp.BT_wrappers

struct Params { }
(
  yarp.name = "yarp::os::Property"
  yarp.includefile="yarp/os/Property.h"
)


/**
 * Enumerates the states every node can be in after execution during a particular
 * time step:
 * - "BT_SUCCESS" indicates that the node has completed running during this time step;
 * - "BT_FAILURE" indicates that the node has determined it will not be able to complete
 *   its task;
 * - "BT_RUNNING" indicates that the node has successfully moved forward during this
 *   time step, but the task is not yet complete;
 * - "BT_IDLE" indicates that the node hasn't run yet.
 * - "BT_HALTED" indicates that the node has been halted by its parent.
 * - "BT_ERROR" indicates the something wrong happened.
 */
enum ReturnStatus {BT_IDLE, BT_RUNNING, BT_SUCCESS, BT_FAILURE, BT_HALTED, BT_ERROR}


/**
 * The ActionID is a small structure useful to uniquely identify the client requesting
 * the action, so that the server can safely handle multiple clients at the same time.
 *
 * Fields are:
 * target: this identifies the main target the action is referred to.
 *         For example for a navigation action, the target is the location to reach.
 * resources: this identifies which resources the action will need to use,
 *         For example for a grasping action this may indicate to perform the actio
 *         with left or right arm.
 * action_ID: an ID number which is required to be unique for each Behaviour Tree for
 *         each node. In case the same action node is repeated more than once, each
 *         instance needs to have a different ID.
 */
struct ActionID {
    1: string target;
    2: string resources;
    3: i32 action_ID;
}

service BT_request {

    /**
     * request_status  Get the status of the action on the server side.
     *
     * target        The target whose state is required
     *                      NOTE: in case the Tick uses a target, then the request_status action MUST use the same target.
     *                      Ex.: For navigation target may be the destination location name.
     * return              The enum indicating the status of the action on the server side.
     */
    ReturnStatus request_status(1:ActionID target);

    /**
     * request_tick  Send a Tick request to the server, along with its parameters.
     *
     * target        The ActionID is a small structure useful to uniquely identify the client requesting the action,
     *                      so that the server can safely handle multiple clients at the same time.
     *                      Optional in case the action has no targets.
     * params        Any addictional parameter the server may require to perform the action.
     *                      Ex.: For navigation target may be the destination location name;
     *                      the params may contain the max speed.
     * return               The enum indicating the status of the action on the server side.
     */
    ReturnStatus request_tick  (1:ActionID target, 2: Params params = {});


    /**
     * request_halt  Send a Halt request to the server, along with its parameters.
     *
     * target        The ActionID is a small structure useful to uniquely identify the client requesting the action,
     *                      so that the server can safely handle multiple clients at the same time.
     *                      Optional in case the action has no targets.
     *                      NOTE: in case the Tick uses a target, then the Halt action MUST use the same target to stop it.
     * params        Any addictional parameter the server may require to perform the action.
     *
     *                      Ex.: For navigation target may be the destination location name;
     *                      the params may contain the max deceleration to apply.
     * return              The enum indicating the status of the action on the server side.
     */
     ReturnStatus request_halt  (1:ActionID target, 2: Params params = {});


    /**
     *  request_initialize  Hook for an initialization callback. The client can ask the action server to perform
     *                      an initialization step.
     * return              true if initialization was successful, false otherwise
     */
    bool request_initialize();

    /**
     * request_terminate  The client notifies the server to close, in order to perform a graceful shutdown.
     * return              true if the call was successful, false otherwise
     */
    bool request_terminate ();
}
