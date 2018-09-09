const control = require('./control')
const pathToPoints = require('./pathToPoints')
const SerialPort = require('serialport')
const port = new SerialPort('/dev/cu.usbmodem14301', {
  baudRate: 57600
})

const path = 'M1,1 L1,101 L101,101 C67.6666667,101 51,84.3333333 51,51 C51,17.6666667 67.6666667,1 101,1 C101,34.3333333 84.3333333,51 51,51 C17.6666667,51 1,34.3333333 1,1 Z'

const points = pathToPoints(path)

const commands = points.map((v2, i) => {
  const v1 = i === 0 ? {x: 1, y: 0} : points[i - 1]

  const d1 = Math.sqrt(Math.pow(v1.x, 2) + Math.pow(v1.y, 2))
  const d2 = Math.sqrt(Math.pow(v2.x, 2) + Math.pow(v2.y, 2))
  const n1 = {
    x: v1.x / d1,
    y: v1.y / d1
  }
  const n2 = {
    x: v2.x / d2,
    y: v2.y / d2
  }

  const degrees = (Math.atan2(v2.y,v2.x) - Math.atan2(v1.y,v1.x)) / Math.PI * 180

  const movement = control.move(d1)
  const rotation = control.rotate(degrees)
  return [
    `${rotation.join(',')}\r`,
    `${movement.join(',')}\r`
  ]
}).reduce((a, b) => a.concat(b), []).filter(d => d !== '0,0,0,0\r')

console.log(commands)

port.on('data', function (data) {
  console.log(data.toString('utf8').trim())
  const input = data.toString('utf8').trim()
  if (isNaN(+input)) {
    console.log(input)
    return
  }
  const i = +input + 1
  if (i < commands.length) {
    console.log(`${i}/${commands.length}`)
    sendCommand(i)
  } else if (i === commands.length){
    console.log(`${i}/${commands.length}`)
    port.write('0,0,0,0\r')
  } else if (i === commands.length + 1){
    console.log('complete')
  }
})

function sendCommand (i = 0) {
  port.write(commands[i])
}

sendCommand()
