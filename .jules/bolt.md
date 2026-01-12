## 2024-05-23 - Display Update Throttling
**Learning:** Sending the entire 1KB buffer to an SSD1306 OLED display over I2C (even at 400kHz) takes ~25ms. Doing this on every loop iteration (~40ms loop) consumes >50% of CPU/Bus time for redundant updates.
**Action:** Implement a "Dirty Flag" pattern (`displayNeedsUpdate`) to only push display frames when state actually changes (inputs, sensors, timers).
