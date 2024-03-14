print("[lua] run lua main.lua")

local serviceId
local ping1
local ping2
local pong
function OnInit(id)
    serviceId = id
    print("[lua] main OnInit id: "..serviceId)

    ping1 = sunnet.AddService("ping")
    print("[lua] new service ping1: "..ping1)

    ping2 = sunnet.AddService("ping")
    print("[lua] new service ping2: "..ping2)

    pong = sunnet.AddService("ping")
    print("[lua] new service pong: "..pong)

    sunnet.Send(ping1, pong, "hi")
    sunnet.Send(ping2, pong, "hello")
end

function OnExit()
    print("[lua] main OnExit id: "..serviceId)

    sunnet.RemoveService(ping1)
    print("[lua] kill service ping1: "..ping1)

    sunnet.RemoveService(ping2)
    print("[lua] kill service ping2: "..ping2)

    sunnet.RemoveService(pong)
    print("[lua] kill service pong: "..pong)
end