## 2026-02-15 - Unthrottled OLED Rendering
**Learning:** Found `drawScreen()` called in every loop iteration (approx 50Hz). Sending 128x64 buffer over I2C (100kHz) takes ~92ms, meaning the loop was effectively blocked by the display refresh, reducing the loop frequency drastically and wasting >80% of CPU time.
**Action:** Always check `loop()` for heavy I2C operations running unthrottled. Throttle display updates to human-perceptible rates (5-10Hz) to free up the main loop for sensors and logic.
