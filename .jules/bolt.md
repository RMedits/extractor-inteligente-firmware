## 2024-05-23 - Sensor Blocking Delay
**Learning:** The `Adafruit_AHTX0` library's `getEvent()` method can be blocking (~80ms). Calling this inside the main UI loop (throttled at 200ms) causes significant UI lag (40% duty cycle of blockage).
**Action:** Always decouple sensor readings from UI/Input loops using separate timers, especially for I2C sensors that might block during measurement.
