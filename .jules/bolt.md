## 2024-05-22 - [Missing Sensor Throttling]
**Learning:** High-latency I2C sensor reads (AHT20/BMP280) were coupled to the 5Hz UI loop, causing unnecessary blocking and I2C traffic.
**Action:** Decouple sensor polling frequency (0.5Hz) from UI refresh rate (5Hz) using separate timers.
