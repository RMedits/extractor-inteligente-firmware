## 2024-05-22 - Display Rendering Optimization
**Learning:** The main loop was redrawing the OLED display on every iteration (~20ms + logic time). Sending a 1KB buffer over I2C at 400kHz takes roughly 25ms. This essentially capped the loop rate and wasted CPU cycles/bus bandwidth on redundant updates.
**Action:** Implement a "Dirty Flag" pattern (`displayNeedsUpdate`). Only trigger `display.display()` when state (sensors, mode, timer, user input) actually changes. This decouples logic frequency from render frequency.
