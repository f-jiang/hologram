'use strict';

(function() {
  const SERIAL_PORT = process.argv[2];  // node index.js /dev/ttyUSB0

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
  var serialPort = new SerialPort(SERIAL_PORT, {baudrate: 9600});

  serialPort.on('open', () => {
    console.log('opened connection to serial port', SERIAL_PORT);

    io.sockets.on('connection', (socket) => {
      console.log('connection with client opened');

      serialPort.on('data', (data) => {
        console.log('reading:', data[0]);
        //socket.emit('reading', data[0]);
        socket.emit('reading', 0.1);
      });
    });
  });

  server.listen(8080);
})();

