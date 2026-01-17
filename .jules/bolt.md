# Bolt's Journal ⚡

## 2024-05-22 - [Initial Discovery]
**Learning:** The AHT20 sensor library has a blocking delay of ~80ms in `getEvent()`.
**Action:** Always throttle sensor reads separate from UI loops to avoid blocking user interaction.
