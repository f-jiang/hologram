'use strict';

(function() {
  document.addEventListener('DOMContentLoaded', () => {
    var iframe = document.getElementById('api-frame');
    var version = '1.0.0'
    var urlid = '375ae80dc7e34b229cc9739b1ce92e2b';

    var client = new Sketchfab(version, iframe);

    var xhttp = new XMLHttpRequest();
    var data;
    var getReadings = function() {
      xhttp.open('GET', 'http://localhost:8080', true);
      xhttp.send();
    };
    xhttp.onreadystatechange = function() {
      if (xhttp.readyState === 4 && xhttp.status === 200) {
        data = JSON.parse(xhttp.responseText).data;
        getReadings();
      }
    };

    var onSuccess = function(api) {
      api.start();
      api.addEventListener('viewerready', () => {
        console.log('Viewer ready');
        api.getNodeMap((err, nodes) => {
          setTimeout(function() {
            var curNode;
            for (var id in nodes) {
              curNode = nodes[id];
              if (curNode.type === 'MatrixTransform') {
                api.rotate(curNode.instanceID, [Math.PI * 3 / 2, 1, 0, 0], 1.0, 'easeOutQuad', function(err, rotateTo) { 
                  console.log(err ? err : 'rotation complete')
                });
              }
            }
          }, 5000);
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

