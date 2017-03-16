'use strict';

(function() {
  document.addEventListener('DOMContentLoaded', () => {
    var iframe = document.getElementById('api-frame');
    var version = '1.0.0'
    var urlid = '375ae80dc7e34b229cc9739b1ce92e2b';

    var client = new Sketchfab(version, iframe);

    var onSuccess = function(api) {
      var xhttp = new XMLHttpRequest();
      var getReadings = function() {
        xhttp.open('GET', 'http://localhost:8080', true);
        xhttp.send();
      };
      xhttp.onreadystatechange = function() {
        if (xhttp.readyState === 4 && xhttp.status === 200) {
          console.log(xhttp.responseText);
          getReadings();
        }
      };

      api.start();
      api.addEventListener('viewerready', () => {
        console.log('Viewer ready');
        getReadings();
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

