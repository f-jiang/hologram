'use strict';

(function() {
  var http = require('http');

  var SerialPort = require("serialport");
  var serialPort = new SerialPort("/dev/ttyUSB0", {baudrate: 9600});

  var readings;

  serialPort.on('open', () => {
    serialPort.on('data', (data) => {
      console.log(data[0]);
      readings = data;
    });
  });

  http.createServer((req, res) => {
    res.on('error', (err) => {
      console.error(err);
      res.statusCode = 400;
      res.end();
    });

    res.statusCode = 200;
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Content-Type', 'text/json');
    res.write(JSON.stringify(readings));
    res.end();
  }).listen(8080, 'localhost');
})();

