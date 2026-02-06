## 2024-05-22 - [Critical Regression: Coupled Sensor/UI Loop]
**Learning:** The sensor reading logic (specifically AHT20 with its 80ms delay) was found tightly coupled to the 200ms UI update loop. This causes guaranteed UI stuttering/lag, negating the benefits of the non-blocking architecture.
**Action:** Decouple sensor reads into a separate low-frequency timer (e.g., 2000ms) independent of the UI loop.
