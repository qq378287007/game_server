print("[lua] run lua ping.lua")

local serviceId
function OnInit(id)
    serviceId = id
    print("[lua] ping OnInit id: "..serviceId)
end

function OnMsg(from, to, buff)
    print("[lua] ping OnMsg id: "..serviceId)
    
    print("[lua] from "..from.." to "..to..", receieve: "..buff)

    if string.len(buff) > 10 then
        sunnet.RemoveService(serviceId)
        return
    end

    sunnet.Send(to, from, buff.."0")
end

function OnExit()
    print("[lua] ping OnExit id: "..serviceId)
end