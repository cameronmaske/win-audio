var audio = require('./src/index')


const devices = audio.getAllDevices()

console.log(devices)

const volume = audio.getVolumeByDevice(devices[0].id)

console.log(volume)

audio.setVolumeByDevice(devices[0].id, 10)
audio.setVolumeByDevice(devices[0].id, 100)
