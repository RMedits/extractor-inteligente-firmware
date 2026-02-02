## 2024-05-23 - Blocking Sensor Reads & UI Latency
**Learning:** The `Adafruit_AHTX0` library's `getEvent()` method contains a blocking `delay(80)`. Calling this inside the main UI loop (e.g., every 200ms) causes significant jitter and sluggishness.
**Action:** Always decouple slow I2C sensor readings from the high-frequency UI loop. Use a separate timer (e.g., 2000ms) for sensors and keep the UI loop (buttons, display) running faster (e.g., 200ms or event-driven). Use `unsigned long lastRead = -interval;` to force immediate execution on boot.
