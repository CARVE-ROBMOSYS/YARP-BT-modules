/**
 *  Define message to talk with remote Balckboard
 *  The Blackbord is a disctionary based storage, where the key is
 *  a string, called 'target' and the value is a yarp::os::Property,
 *  which again is a dictionary. This allows maximum flexibility in data
 *  to be stored and easy to use retrieval.
 */

namespace yarp yarp.BT_wrappers

struct Data { }
(
  yarp.name = "yarp::os::Property"
  yarp.includefile="yarp/os/Property.h"
)

service BlackBoardWrapper {
    Data getData(1: string target)
    bool setData(1: string target, 2: Data datum)
    list<string> listTarget()
    void clearData()
    void initializeData()
}
