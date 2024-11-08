local Role = function()
    local M = {
        id = -1,    --Actor标识
        coin = 100,
        hp = 200,
    }

    function M:dispatch(source, msg)
        if msg == "work" then
            self.coin = self.coin + 10
            print(self.id.." work, coin:"..self.coin)
            send(self.id, source, "work")
        elseif msg == "eat" then
            self.hp = self.hp + 5
            print(self.id.." eat, hp:"..self.hp)
        else
            --更多消息处理
        end
    end

    return M
end

newactor(101, Role())
newactor(102, Role())
newactor(103, Role())
newactor(104, Role())
send(101, 102, "work")
send(103, 104, "eat")
