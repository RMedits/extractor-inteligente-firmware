## 2024-05-23 - AHT20 Blocking Delay
**Learning:** The `Adafruit_AHTX0` library's `getEvent()` method contains a blocking `delay(80)` to wait for measurement completion. Calling this inside a high-frequency display loop (e.g., 150ms) causes the CPU to sleep for >50% of the time, killing responsiveness.
**Action:** Always decouple sensor readings (slow, < 1Hz) from UI/Input loops (fast, > 10Hz). Use a non-blocking timer pattern to read sensors only when necessary and cache the values.
