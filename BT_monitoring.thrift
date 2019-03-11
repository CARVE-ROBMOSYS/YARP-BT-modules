# BTCmd.thrift
#

include "BTCmd.thrift"

enum Direction {REQUEST, REPLY}

struct BTMonitorMsg {
  1: string source;
  2: string target;
  3: string event;
}
