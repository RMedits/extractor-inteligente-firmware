# Bolt's Journal

## 2026-01-29 - Decoupling Sensor I/O from UI Loop
**Learning:** The main loop architecture coupled heavy I2C sensor readings (AHT20/BMP280) to the display refresh rate (5Hz). This caused potential UI lag due to blocking I2C calls or measurement delays (e.g., AHT20 ~80ms).
**Action:** Decouple slow I/O operations into their own throttled timers (e.g., 2000ms) independent of the UI loop to ensure interface responsiveness.
