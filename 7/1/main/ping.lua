print("[lua] run lua ping.lua")

local serviceId
function OnInit(id)
    serviceId = id
    print("[lua] ping OnInit id: "..serviceId)
end

function OnExit()
    print("[lua] ping OnExit id: "..serviceId)
end