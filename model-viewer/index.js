'use strict';

(function() {
  var http = require('http');
  var https = require('https');
  var fs = require('fs');
  var express = require('express');
  var path = require('path');
  var SerialPort = require('serialport');
  var url = require('url');

  const SERIAL_PORT = process.argv[2];  // node index.js /dev/ttyUSB0
  const HTTP_PORT = 8080;
  const APP_DIR = path.join(__dirname, 'frontend');

  var app = express();
  var server = http.Server(app);
  var io = require('socket.io')(server);
  var serial = new SerialPort(SERIAL_PORT, {baudrate: 9600});

  var map = function(val, fromLo, fromHi, toLo, toHi) {
    return toLo + (val - fromLo) * (toHi - toLo) / (fromHi - fromLo);
  };

  serial.on('open', () => {
    console.log('opened connection to serial port', SERIAL_PORT);

    io.sockets.on('connection', (socket) => {
      console.log('connection with client opened');

      var angle, elevation, buf;
      serial.on('data', (data) => {
        buf = Buffer.from(data);
        angle = map(buf.readUInt16LE(0), 0, 320, 0, 2 * Math.PI);
        elevation = map(buf.readUInt16LE(2), 0, 1024, 0, 1);

        console.log('angle: ' + angle, 'elevation: ' + elevation);
        socket.emit('readings', {'angle': angle, 'elevation': elevation});
      });
    });
  });

  new Promise((resolve, reject) => {
    fs.readFile('models.json', (err, contents) => {
      var urlids = JSON.parse(contents);
      var count = 0;
      var models = [];

      for (var i = 0; i < urlids.length; i++) {
        https.get('https://api.sketchfab.com/v3/models/' + urlids[i], (res) => {
          var body = '';

          res.on('data', (d) => {
            body += d;
          });

          res.on('end', () => {
            models.push(JSON.parse(body));
            
            if (++count === urlids.length) {
              resolve(models);
            }
          });
        });
      }
    });
  }).then((data) => {
    app.use(express.static(APP_DIR));

    app.get('/', (req, res) => {
      res.sendFile(path.join(APP_DIR, 'app.html'));
    });

    app.get('/thumbnails', (req, res) => {
      var thumbnails = [];

      for (var i = 0; i < data.length; i++) {
        thumbnails.push(data[i].thumbnails.images[0].url);
      }

      res.json(thumbnails);
    });

    app.get('/urlid', (req, res) => {
      var index = req.query.index;

      if (index < data.length && index >= 0) {
        res.send(data[index].uid);
      } else {
        res.status(404).send('urlid not found');
      }
    });

    server.listen(HTTP_PORT, (err) => {
      if (err) {
        console.log('server unable to listen on port', HTTP_PORT);
      } else {
        console.log('server listening on port', HTTP_PORT);
      }
    });
  }).catch((reason) => {
    console.log(reason);
  });
})();

