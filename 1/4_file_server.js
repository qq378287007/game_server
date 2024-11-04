var net = require('net');
const fs = require('fs')

var server = net.createServer(function (socket) {
    var data = fs.readFileSync('save.txt')
    socket.on('close', function () {
        fs.writeFileSync('save.txt', data)
    });
});
server.listen(8002)

//telnet 127.0.0.1 8002
