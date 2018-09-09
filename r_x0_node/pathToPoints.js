const path = require('svg-path-properties')

module.exports = function (p, resolution = 99) {
  const props = path.svgPathProperties(p)
  const length = props.getTotalLength()
  const offset = props.getPointAtLength(0)

  const points = '.'.repeat(resolution).split('').map((d, i) => {
    return props.getPointAtLength(length / (resolution - 1) * i)
  })

  return points.filter((p, i) => i > 0).map((p, i) => {
    return {
      x: p.x - points[i].x,
      y: p.y - points[i].y
    }
  })
}
