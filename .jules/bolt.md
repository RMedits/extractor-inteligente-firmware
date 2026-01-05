# Bolt's Journal

## 2024-05-22 - Initial Assessment
**Learning:** The application uses an ESP32 with AHT20, BMP280, and MQ135 sensors, plus an SSD1306 OLED display.
**Action:** Identified potential bottlenecks in display rendering (`display.display()` is blocking and called every loop iteration) and sensor reading (blocking delays in library calls).

## 2024-05-22 - Display Rendering Bottleneck
**Learning:** The `drawAutoScreen`, `drawManualRunScreen`, etc., call `display.display()` which sends the entire buffer over I2C. At 400kHz, a 128x64 screen (1KB) takes roughly ~20-25ms. Doing this on every loop iteration, even if data hasn't changed, is wasteful.
**Action:** Implement a "dirty flag" mechanism to only update the display when data actually changes (sensor readings updated, mode changed, user interaction).
