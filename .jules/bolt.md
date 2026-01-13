# Bolt's Journal

## 2024-05-23 - Redundant Display Rendering
**Learning:** The I2C display update (`display.display()`) takes ~25ms and was being called in every iteration of the main loop, regardless of whether any data had changed. This capped the loop frequency at ~22Hz and wasted CPU cycles.
**Action:** Implement a "Dirty Flag" pattern (`displayNeedsUpdate`) to only render frames when state (sensors, timer, inputs) actually changes. This should free up significant CPU time for other tasks.
