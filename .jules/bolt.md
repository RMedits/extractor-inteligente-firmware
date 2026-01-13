# BOLT'S JOURNAL

## 2024-05-22 - OLED Display Blocking I2C Bus
**Learning:** The `Adafruit_SSD1306` library's `display()` method transmits the entire frame buffer (1024 bytes) over I2C. At 400kHz, this takes ~23ms minimum. Calling this on every loop iteration (50Hz) saturates the I2C bus and consumes ~50-80% of CPU time in blocking I/O, increasing input latency.
**Action:** Decouple display rendering from the main loop logic. Update the display at a fixed lower framerate (e.g., 10-20 FPS) or only when dirty. This frees up the main loop for high-frequency input polling (encoders/buttons).
