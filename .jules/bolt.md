## 2026-01-26 - Decoupling Sensor Loop from UI Loop
**Learning:** High-latency I2C sensor reads (AHT20 takes ~80ms) coupled to a 200ms UI refresh loop cause 40% of the UI time to be blocked waiting for sensors. This makes the interface feel sluggish and increases bus traffic unnecessarily.
**Action:** Decouple sensor polling from UI updates. Run sensors on a slower, independent timer (e.g., 2000ms) and cache the values for the faster UI loop (200ms) to display.
