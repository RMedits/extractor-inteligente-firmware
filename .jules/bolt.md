## 2024-05-23 - [Display I2C Bottleneck]
**Learning:** The SSD1306 `display.display()` call transmits the full 128x64 buffer (1024 bytes) over I2C every single loop iteration. At 400kHz, this takes ~25ms plus overhead, causing significant CPU blocking.
**Action:** Implement a 'Dirty Flag' pattern (`displayNeedsUpdate`) to only re-render and transmit the frame buffer when the state actually changes (sensor update, user input, timer tick).
