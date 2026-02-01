## 2024-05-22 - [Sensor Polling Bottleneck]
**Learning:** The AHT20 and BMP280 sensors were being polled at 5Hz (inside the 200ms UI loop), causing unnecessary I2C traffic and potential UI jitter due to blocking library calls (e.g. `getEvent`).
**Action:** Decouple sensor logic from UI/Display logic. Use separate timers: fast for UI (e.g. 200ms) and slow for environment sensors (e.g. 2000ms).
