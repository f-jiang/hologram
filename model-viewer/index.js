'use strict';

(function() {
  const SERIAL_PORT = process.argv[2];  // node index.js /dev/ttyUSB0

  var map = function(val, fromLo, fromHi, toLo, toHi) {
    return toLow + (val - fromLo) * (toHi - toLo) / (fromHi - fromLo);
  };

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

      var angle, tilt, buf;
      serialPort.on('data', (data) => {
        buf = Buffer.from(data);
        angle = map(buf.readUInt16LE(0), 0, 320, 0, 2 * Math.PI);
        tilt = map(buf.readUInt16LE(2), 0, 1024, 0, 2 * Math.PI);

        console.log('angle: ' + angle, 'tilt: ' + tilt);
        socket.emit('readings', {'angle': angle, 'tilt': tilt});
      });
    });
  });

  server.listen(8080);
})();
