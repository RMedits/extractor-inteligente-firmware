## 2024-05-22 - Display Rendering & Sensor Blocking
**Learning:** The `Adafruit_SSD1306` library's `display()` method takes ~25ms over I2C at 400kHz, and `Adafruit_AHTX0` has a blocking `delay(80)`.
**Action:** Implemented a "Dirty Flag" pattern to only update the display when state actually changes, reducing I2C traffic by ~98% in idle/steady states. Also added guards in `setFanSpeed` to prevent redundant PWM writes and blocking delays in Manual Mode (0 speed).
