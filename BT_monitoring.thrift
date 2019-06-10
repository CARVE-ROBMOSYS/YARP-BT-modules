# BTCmd.thrift
#

include "BTCmd.thrift"

enum Direction {REQUEST, REPLY}

struct BTMonitorMsg {
  1: string skill;
  2: string event;
}
