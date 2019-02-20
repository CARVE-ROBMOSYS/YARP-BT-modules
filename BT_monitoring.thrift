# BTCmd.thrift
#

include "BTCmd.thrift"

enum Direction {REQUEST, REPLY}

struct BTMonitorMsg {
  1: string requestor;
  2: string target;
  3: Direction direction;
  4: BTCmd.TickCommand command;
  5: BTCmd.ReturnStatus reply;
}
