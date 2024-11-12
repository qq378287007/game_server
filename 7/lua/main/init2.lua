print("run lua init.lua")

function OnInit(id)
    print("[lua] main OnInit id: "..id)

    local ping1 = sunnet.NewService("ping")
    print("[lua] new service ping1: "..ping1)

    local ping2 = sunnet.NewService("ping")
    print("[lua] new service ping2: "..ping2)

    local pong = sunnet.NewService("pong")
    print("[lua] new service pong: "..pong)

    local num = sunnet.NewService(1111)
    print("[lua] new service num: "..num)

    local unknown = sunnet.NewService(nil)
    print("[lua] new service unknown: "..unknown)
end

function OnExit()
    print("[lua] main OnExit")
end