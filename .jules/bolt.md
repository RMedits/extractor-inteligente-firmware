## 2024-05-23 - Sensor Throttling
**Learning:** Moving blocking I2C sensor reads (specifically AHT20 with its 80ms delay) from the main UI loop (200ms) to a separate throttled timer (2000ms) eliminates periodic UI stutter and massively improves responsiveness.
**Action:** Always decouple slow I2C devices from the high-frequency UI/Animation loop. Use a separate `lastSensorRead` timer.
