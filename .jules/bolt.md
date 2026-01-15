## 2024-05-23 - I2C OLED Display Blocking
**Learning:** The `Adafruit_SSD1306` `display()` method transmits the entire 1KB buffer over I2C, which takes ~25ms at 400kHz. Calling this on every loop iteration saturates the bus (~40Hz max) and leaves little CPU time for other tasks.
**Action:** Always implement a frame rate limiter (e.g., 30 FPS) or a "dirty flag" system for I2C displays to prevent blocking critical logic like button debounce or PWM ramping.
