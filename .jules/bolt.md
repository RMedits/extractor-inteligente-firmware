## 2024-05-23 - Sensor Read Decoupling
**Learning:** I2C sensor reads (like AHT20/BMP280) are blocking and can take ~80ms+, causing severe UI lag if placed in the main display loop (200ms).
**Action:** Always decouple slow sensor reads from the UI loop using a separate timer (e.g., 2000ms) to maintain responsiveness.
