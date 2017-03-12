var SerialPort = require("serialport");
var serialport = new SerialPort("\\\\.\\COM3", {baudrate: 9600});
serialport.on('open', function(){
  console.log('Serial Port Opened');
  serialport.on('data', function(data){
      console.log(data[0]);
  });
});