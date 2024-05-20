var net = require('net');

var server = net.createServer(function (socket) {
    console.log('connected, port: ' + socket.remotePort);

    //接收到数据
    socket.on('data', function (data) {
        console.log('client send: ' + data);
        var ret = "\r\nserver recv: " + data + "\r\n";
        socket.write(ret);
    });

    //断开连接
    socket.on('close', function () {
        console.log('closed, port: ' + socket.remotePort);
    });
});
server.listen(8002);

//telnet 127.0.0.1 8002
