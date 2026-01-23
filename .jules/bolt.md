# Bolt's Performance Journal

## 2024-05-23 - Sensor Polling Bottleneck
**Learning:** Reading I2C sensors (AHT20, BMP280) inside the main UI loop (every 200ms) causes unnecessary bus traffic and CPU blocking, especially if the sensor library has delays.
**Action:** Decouple sensor polling from UI updates. Poll sensors at a slower rate (e.g., 2000ms) appropriate for physical changes, while keeping UI responsive.
