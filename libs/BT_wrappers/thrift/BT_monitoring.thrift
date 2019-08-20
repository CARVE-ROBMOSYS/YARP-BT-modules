# BTCmd.thrift
#

namespace yarp BT_wrappers

include "BTCmd.thrift"

enum Direction {REQUEST, REPLY}

struct BTMonitorMsg {
  1: string skill;
  2: string event;
}
