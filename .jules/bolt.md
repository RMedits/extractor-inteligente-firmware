## 2024-05-23 - Blocking Delays in Main Loop
**Learning:** The `loop()` contained `delay(300)` for button debouncing inside a throttled block. This freezes the entire system (sensors, display, inputs) for 300ms on every press, causing poor responsiveness.
**Action:** Always use non-blocking `millis()` checks for debouncing and decouple fast input polling from slow display/sensor updates.

## 2024-05-23 - I2C Throttling
**Learning:** Display updates (SSD1306) are expensive on I2C. Updating them at the same rate as sensor reads or faster than needed (e.g., >4Hz) wastes CPU and bus time.
**Action:** Limit display updates to ~4Hz (250ms) independently of input polling.
