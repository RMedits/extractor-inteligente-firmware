# Bolt's Journal

## 2024-05-22 - Main Loop Coupling
**Learning:** The main application loop tightly coupled input handling, sensor reading, and display updates to a single 150ms timer. This created artificial latency for user inputs and excessive I2C traffic for sensors.
**Action:** Decouple these systems: run inputs fast (non-blocking), display at 4Hz, and sensors at 0.5Hz.
