var net = require('net');

var scenes = new Map();

var server = net.createServer(function (socket) {//新连接
    scenes.set(socket, true) 

    socket.on('data', function (data) { //收到数据
        var id = socket.remotePort;
        for (let s of scenes.keys()) {
            s.write("\r\n" + id + ": " + data + "\r\n");
        }
    });
/*
    socket.on('close', function () {//断开连接
        scenes.delete(socket)
    });
*/
});

server.listen(8010);

//telnet 127.0.0.1 8010
