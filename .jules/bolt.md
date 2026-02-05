# BOLT'S JOURNAL - CRITICAL LEARNINGS

## 2024-05-22 - Sensor Blocking Delay
**Learning:** The `Adafruit_AHTX0` library introduces a blocking `delay(80)` in `getEvent()`. Calling this inside the main 200ms UI loop caused significant frame drops and unresponsiveness.
**Action:** Decoupled sensor reading from the UI loop. Sensors are now polled every 2000ms, independent of the 200ms display refresh rate.
