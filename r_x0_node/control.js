const radius = 76 / 2
const spread = 240
const steps = 1600

const steps_per_mm = steps / (radius * 2 * Math.PI)
const steps_per_deg = (spread * Math.PI) / 360 / (1 / steps_per_mm)

function rotate (deg = 0, left = true) {
  if (left === false) deg = -deg
  const steps = Math.round(deg * steps_per_deg)
  return [1, steps > 0 ? 1 : 0, steps > 0 ? 1 : 0, Math.abs(steps)]
}

function move (mm = 0, forward = true) {
  const steps = Math.round(mm * steps_per_mm)
  return [1, forward ? 1 : 0, forward ? 0 : 1, steps]
}

module.exports = { rotate, move }
