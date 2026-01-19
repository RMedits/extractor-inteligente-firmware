## 2024-05-22 - Sensor Blocking Jitter
**Learning:** The `Adafruit_AHTX0::getEvent()` method contains a blocking `delay(80)`. Calling this within the main 200ms display loop consumes ~40% of the available frame time, creating significant overhead and potential UI jitter.
**Action:** Always decouple blocking sensor reads from the UI loop using a separate timer (e.g., 2000ms), caching the values for the faster display loop.
