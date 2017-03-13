const {app, BrowserWindow} = require('electron')
const path = require('path')
const url = require('url')

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let win

var iframe = $( '#api-frame' )[ 0 ];
var version = '1.0.0';
var urlid = 'uxszTZETjcYGoKcS1RZlsAa92ZT';

function createWindow () {
  // Create the browser window.
  win = new BrowserWindow({width: 800, height: 600})

  // and load the index.html of the app.
  win.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  // Open the DevTools.
  win.webContents.openDevTools()

  // Emitted when the window is closed.
  win.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    win = null
  })
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow)

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', () => {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (win === null) {
    createWindow()
  }
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.

var client = new Sketchfab( version, iframe );

var error = function () {
    console.error( 'Error api Sketchfab !' );
};

var success = function ( api ) {

    api.start( function () {

        //Be careful, the mouse can't be detected on the iframe
        //If you want to fix this, you have to put an overlay on top of the iframe
        $( 'body' ).mousemove( function ( event ) {

            // Calculate the location of the middle of the frame (Where we want the model to stay)
            var frameX = $( '#api-frame' ).position().left + $( '#api-frame' ).width() / 2;
            var frameY = $( '#api-frame' ).position().top + $( '#api-frame' ).height() / 2;

            var x = event.pageX - frameX;
            var y = event.pageY - frameY;
            var z = 2;

            // Calculate the distance, normalize the vector by dividing by distance and multiplying by a factor
            var distance = Math.sqrt( Math.pow( x, 2 ) + Math.pow( y, 2 ) + Math.pow( z, 2 ) );
            x = ( x / distance ) * 6;
            y = ( y / distance ) * 6;
            z = ( z / distance ) * 6;

            // x, y and z depend on how the model was made
            api.lookat( [ y, -x, z ], [ 0, 0, 0 ], 0 );
        } );

    } );
};

client.init( urlid, {
    success: success,
    error: error
} );
