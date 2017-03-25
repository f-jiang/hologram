'use strict';

(function() {
  $(document).ready(() => {
    var iframe = $('#api-frame')[0];
    var version = '1.0.0'

    var client = new Sketchfab(version, iframe);
    var socket = io.connect('http://localhost:8080');

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

        socket.emit();  // open connection with server

        var dist = 0;
        var x = 0;
        var y = 0;
        var z = 0;
        var target = [0, 0, 0];
        socket.on('readings', (readings) => {
          if (readings.angle === 0) {
            readings.angle = 0.001;
          }

          console.log('angle: ' + readings.angle, 'tilt: ' + readings.tilt);

          api.getCameraLookAt((err, camera) => {
            dist = Math.sqrt(Math.pow(getDist(camera.position, target), 2) -
              Math.pow(z - target[2], 2));
            x = target[0] + dist * Math.cos(readings.angle);
            y = target[1] + dist * Math.sin(readings.angle);
            z = target[2] + dist * Math.sin(readings.tilt);
            api.lookat([x, y, z], target, 0);
          });
        });
      });
    };

    var onError = function() {
      console.log('Sketchfab API error. Maybe you\'re not online?');
    };

    var loadModel = function(index) {
      $.get('/urlid?index=' + index, (urlid) => {
        client.init(urlid, {
          success: onSuccess,
          error: onError,
          camera: 0,
          preload: 1,
          navigation: 'fps'
        });
      });
    };

    $.get('/thumbnails', (thumbnails) => {
      for (var i = 0; i < thumbnails.length; i++) {
        $('.gallery').append(`
          <div class="slide">
            <img class="thumbnail" src="${thumbnails[i]}"/>
          </div>
        `);
      }

      $('.drawer').drawer();

      var start = 0;
      $('.gallery').slick({
        accessibility: false,
        centerMode: true,
        dots: true,
        focusOnSelect: true,
        initialSlide: start,
        slidesToShow: 5,
        slidesToScroll: 1
      });
      $('.gallery').on('afterChange', (event, slick, currentSlide) => {
        loadModel(currentSlide);
      });

      loadModel(start);
    });
  });
})();

