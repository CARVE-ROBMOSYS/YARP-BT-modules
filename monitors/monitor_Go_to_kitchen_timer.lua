--
-- Authors: giuse
-- Version: 1.0.0
-- Created using rFSMGui
--
require "rfsm"
require "rfsm_timeevent"

function gettime() return os.time(), 0 end
rfsm_timeevent.set_gettime_hook(gettime)
return rfsm.state {


    --States

    --OK
    OK = rfsm.state{
		entry = function() print("To_bt sent") end,
        doo = function()    end,
    }, --end of OK


    --FAILED
    FAILED = rfsm.state{
        doo = function() print("Action failed")  end,
    }, --end of FAILED


    --wait
    wait_from_bt = rfsm.state{
			entry = function() print("Wait From_bt") end,
        		doo = function()  end,
    }, --end of wait


    initial = rfsm.conn{ },

    --wait_to_bt
    wait_to_bt = rfsm.state{
		entry = function() print("is_robot_in_kitchen arrived") end,
          doo=function()
        	  end,
    }, --end of wait_to_bt


    --wait_for_req
    wait_for_req = rfsm.state{
		entry = function() print("from_bt arrived") end,
          doo=function()
        	  end,
    }, --end of wait_for_req


    --wait_is_robot
    wait_is_robot = rfsm.state{
        doo = function() print("Req sent")  end,
    }, --end of wait_is_robot



    --Transitions
    rfsm.trans{ src = 'initial', tgt = 'wait_from_bt', pn = 0 },
    rfsm.trans{ src = 'wait_from_bt', tgt = 'wait_for_req', pn = 0, events = {"e_from_bt"} },
    rfsm.trans{ src = 'wait_to_bt', tgt = 'OK', pn = 0, events = {"e_to_bt"} },
    rfsm.trans{ src = 'OK', tgt = 'wait_from_bt', pn = 0 },
    rfsm.trans{ src = 'wait_to_bt', tgt = 'FAILED', pn = 0, events = {"e_after(10)"} },
    rfsm.trans{ src = 'wait_from_bt', tgt = 'FAILED', pn = 0, events = {"e_after(100)"} },
    rfsm.trans{ src = 'wait_for_req', tgt = 'FAILED', pn = 0, events = {"e_after(10)"} },
    rfsm.trans{ src = 'wait_for_req', tgt = 'wait_is_robot', pn = 0, events = {"e_req"} },
    rfsm.trans{ src = 'wait_is_robot', tgt = 'wait_to_bt', pn = 0, events = {"e_is_robot"} },
    rfsm.trans{ src = 'wait_is_robot', tgt = 'FAILED', pn = 0, events = {"e_after(100)"} },
	rfsm.trans{ src = 'FAILED', tgt = 'wait_from_bt', pn = 0, events = {"e_reset"} },
}
