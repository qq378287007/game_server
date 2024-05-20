var net = require('net');

class Role {
    constructor() {
        this.x = 0;
        this.y = 0;
    }
}

var roles = new Map();

var server = net.createServer(function (socket) {//新连接
    roles.set(socket, new Role())

    socket.on('data', function (data) {//接收到数据
        var role = roles.get(socket);
        var cmd = String(data);

        //更新位置
        if (cmd == "left") role.x--;
        else if (cmd == "right") role.x++;
        else if (cmd == "up") role.y--;
        else if (cmd == "down") role.y++;

        for (let s of roles.keys()) {//广播
            var id = socket.remotePort;
            var str = "\r\n" + id + " move to " + role.x + " " + role.y + "\r\n";
            s.write(str);
        }
    });

    socket.on('close', function () {//断开连接
        roles.delete(socket)
    });
});

server.listen(8002);

//telnet 127.0.0.1 8002
