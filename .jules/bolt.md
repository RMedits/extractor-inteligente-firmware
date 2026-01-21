# Bolt's Journal

## 2024-05-22 - [Sensor Throttling Optimization]
**Learning:** The previous main loop architecture was running sensor reads (AHT20, BMP280, MQ135) at 5Hz (every 200ms) inside the display update loop. The AHT20 library often has blocking delays or long integration times, and reading it so frequently is unnecessary and wasteful.
**Action:** Decoupled sensor reading from the UI loop. Sensors now run at 0.5Hz (2000ms), while UI continues at 5Hz. This prevents UI stuttering caused by sensor reads and reduces I2C bus congestion.
