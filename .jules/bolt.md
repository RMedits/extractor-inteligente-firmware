## 2024-10-24 - Sensor Throttling & UI Latency
**Learning:** Reading I2C sensors (AHT20) inside the main 200ms UI loop creates significant blocking delays (~80ms), degrading interface responsiveness.
**Action:** Decouple sensor polling from the UI loop. Use a separate timer (e.g., 2000ms) for sensors to free up CPU cycles for the display and user input. Refactoring inline code to `readSensors()` simplifies this logic.
