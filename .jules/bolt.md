# BOLT'S JOURNAL - CRITICAL LEARNINGS ONLY

## 2024-05-22 - [Decoupled Sensor Reads]
**Learning:** Frequent I2C sensor reads (AHT20) inside the main UI loop (5Hz) block the entire system due to measurement delays (~80ms). This effectively halts the UI for significant portions of time.
**Action:** Always decouple slow I2C sensor readings from high-frequency UI loops using separate non-blocking timers (e.g., standard "Blink Without Delay" pattern).
