## 2024-05-21 - Sensor Blocking & I2C Bottlenecks
**Learning:** The `Adafruit_AHTX0` library introduces a significant blocking delay (~80ms) during `getEvent()`. Coupled with a slow I2C clock (100kHz), reading sensors every 200ms frame effectively halves the available CPU time for UI rendering, causing sluggish response.
**Action:** Always decouple sensor polling from UI loops. Use a separate timer (e.g., 2000ms) for environmental sensors. Increase I2C clock to 400kHz for faster OLED updates.
