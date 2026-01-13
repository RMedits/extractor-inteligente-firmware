# Bolt's Journal

## 2023-10-27 - Display Refresh Rate Throttling
**Learning:** The `Adafruit_SSD1306` library's `display()` function is blocking and transfers the entire buffer (~1KB) over I2C. Running this in every loop cycle (approx every 40ms) consumes significant CPU time and I2C bandwidth, potentially affecting input responsiveness (rotary encoder).
**Action:** Decouple display updates from the main logic loop. Throttle display updates to a fixed rate (e.g., 20FPS / 50ms) to free up resources for input polling and sensor logic without degrading visual quality.
