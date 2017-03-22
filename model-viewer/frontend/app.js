'use strict';

(function() {
  document.addEventListener('DOMContentLoaded', () => {
    var iframe = document.getElementById('api-frame');
    var version = '1.0.0'
    var urlid = '375ae80dc7e34b229cc9739b1ce92e2b';

    var client = new Sketchfab(version, iframe);
//    var socket = io.connect('http://localhost:8080');

    // p1[0] = x1
    // p1[1] = y1
    // p1[2] = z1
    var getDist = function(p1, p2) {
      return Math.sqrt(Math.pow(p2[0] - p1[0], 2)
        + Math.pow(p2[1] - p1[1], 2)
        + Math.pow(p2[2] - p1[2], 2));
    };

    var onSuccess = function(api) {
      console.log('Sketchfab API initialized');

      api.start();
      api.addEventListener('viewerready', () => {
        console.log('viewer ready');

//        socket.emit();  // open connection with server

        var dist, x, y, z;
        var target = [0, 0, 0];
//        socket.on('readings', (readings) => {
//          if (readings.angle === 0) {
//            readings.angle = 0.001;
//          }
//
//          console.log('angle: ' + readings.angle, 'elevation: ' + readings.elevation);
//
//          api.getCameraLookAt((err, camera) => {
//            dist = Math.sqrt(Math.pow(getDist(camera.position, target), 2) -
//              Math.pow(z - target[2], 2));
//            x = target[0] + dist * Math.cos(readings.angle);
//            y = target[1] + dist * Math.sin(readings.angle);
//            z = target[2];
//            api.lookat([x, y, z], target, 0);
//          });
//        });
      });
    };

    var onError = function() {
      console.log('Sketchfab API error. Maybe you\'re not online?');
    };

    $(document).ready(() => {

      $.get('/thumbnails', (data) => {
        for (var i = 0; i < data.length; i++) {
          $('.gallery').append(`
            <div>
              <img src=` + data[i] + `/>
            </div>
          `);
        }

        $('.drawer').drawer();
        $('.gallery').slick({
          accessibility: false,
          centerMode: true,
          dots: true,
          slidesToShow: 3,
          slidesToScroll: 1
        });
        client.init(urlid, {
          success: onSuccess,
          error: onError,
          camera: 0,
          preload: 1,
          navigation: 'fps'
        });
      });
    });
  });
})();

