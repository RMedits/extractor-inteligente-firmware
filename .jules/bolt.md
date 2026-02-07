## 2024-05-22 - [Decoupled Sensor I/O from UI]
**Learning:** The AHT20 sensor library has a blocking delay (~80ms) which freezes the UI when coupled to the display update loop (200ms).
**Action:** Always decouple slow I/O operations (like I2C sensors) from the main UI/display loop using separate non-blocking timers.
