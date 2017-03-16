'use strict';

(function() {
  document.addEventListener('DOMContentLoaded', () => {
    var iframe = document.getElementById('api-frame');
    var version = '1.0.0'
    var urlid = '375ae80dc7e34b229cc9739b1ce92e2b';

    var client = new Sketchfab(version, iframe);
    var socket = io.connect('http://localhost:8080');

    var onSuccess = function(api) {
      api.start();
      api.addEventListener('viewerready', () => {
        console.log('Viewer ready');
        socket.on('reading', (reading) => {
          api.lookat([10 * Math.cos(reading), 10 * Math.sin(reading), 0], [0, 0, 0], 0);
	});
      });
    };

    var onError = function() {
      console.log('Sketchfab API error. Maybe you\'re not online?');
    };

    client.init(urlid, {
      success: onSuccess,
      error: onError
    });
  });
})();

