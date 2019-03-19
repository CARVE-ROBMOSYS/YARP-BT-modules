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
 * - "BT_ERROR" indicates the something wring happened.
 */

enum TickCommand {BT_TICK, BT_STATUS, BT_HALT}
enum ReturnStatus {BT_RUNNING, BT_SUCCESS, BT_FAILURE, BT_IDLE, BT_HALTED, BT_ERROR}

service BTCmd {
  ReturnStatus request_tick(1:string params = "");
  ReturnStatus request_status();
  ReturnStatus request_halt(1:string params = "");
}
