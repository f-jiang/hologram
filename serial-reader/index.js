'use strict';

(function() {
  var http = require('http');
  var fs = require('fs');

  var server = http.createServer((req, res) => {
    fs.readFile('./app.html', 'utf-8', function(error, content) {
      res.writeHead(200, {"Content-Type": "text/html"});
      res.end(content);
    });
  });

  var io = require('socket.io').listen(server);
  var SerialPort = require("serialport");
  var serialPort = new SerialPort("/dev/ttyUSB0", {baudrate: 9600});

  io.sockets.on('connection', (socket) => {
    serialPort.on('open', () => {
      serialPort.on('data', (data) => {
        socket.emit('reading', data);
      });
    });
  });

  server.listen(8080);
})();

