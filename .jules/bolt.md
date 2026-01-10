## 2024-05-23 - [Embedded Display Bottleneck]
**Learning:** Sending a 1KB I2C buffer to an OLED display (128x64) takes ~25ms at 400kHz. Doing this in every loop iteration (even at 50Hz) consumes >50% of CPU time and saturates the bus.
**Action:** Always implement a "Dirty Flag" pattern for embedded displays. Only call `display.display()` when a value (sensor, state, input) has actually changed.
