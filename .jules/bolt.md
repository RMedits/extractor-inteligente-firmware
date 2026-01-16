# Bolt's Journal

## 2024-05-23 - Sensor Throttling
**Learning:** The AHT20 `getEvent()` method blocks for ~80ms. Calling this inside the main loop every 200ms wastes ~40% of the available CPU time on redundant sensor reads.
**Action:** Decouple sensor polling from the main UI/Display loop. Poll sensors at a lower frequency (e.g., 0.5Hz) while keeping UI responsive.
