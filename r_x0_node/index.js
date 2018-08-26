const SerialPort = require('serialport')
const port = new SerialPort('/dev/cu.usbmodem14501', {
  baudRate: 9600
})


port.on('data', function (data) {
  const str = data.toString('utf8').trim()

  if (str === 'START') {
    port.write(`${1},${0},${1},${400}\r`)
    port.write(`${0},${1},${1},${400}\r`)
    port.write(`${1},${0},${1},${800}\r`)
    port.write(`STOP,,,\r`)
  } else if (str === 'SUCCESS') {
    console.log('SUCCESS')
  }
})
