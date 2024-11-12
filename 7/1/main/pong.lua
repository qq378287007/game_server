print("[lua] run lua pong.lua")

local serviceId

function OnInit(id)
    serviceId = id
    print("[lua] pong OnInit id: "..serviceId)
end

function OnExit()
    print("[lua] pong OnExit id: "..serviceId)
end