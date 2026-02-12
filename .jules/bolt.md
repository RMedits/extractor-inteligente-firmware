## 2026-02-12 - Unthrottled OLED updates block main loop
**Learning:** The I2C display update (128x64 buffer) takes ~80-100ms at 100kHz, blocking the `loop()` function. When called unconditionally, the loop frequency drops to ~10Hz, making button inputs sluggish.
**Action:** Throttle display updates to 5Hz (200ms) to free up CPU for inputs and logic.
