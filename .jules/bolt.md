## 2024-05-23 - [Display Throttling]
**Learning:** The OLED display update (`display.display()`) over I2C is a significant blocking operation (~25-50ms) that consumes >50% of the main loop time when running at default speeds. This affects input responsiveness (button polling) and wastes CPU cycles.
**Action:** Decouple display updates from the main logic loop. Use a non-blocking timer to update the screen at a lower frequency (e.g., 5-6 FPS for static content, 20 FPS for interactive menus) while keeping the logic loop running as fast as possible for inputs.
