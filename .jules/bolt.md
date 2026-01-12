## 2024-05-23 - Display Refresh Bottleneck
**Learning:** Hard-coded `delay(20)` combined with blocking `display.display()` (~25ms) capped the main loop at ~20Hz. This made the rotary encoder feel sluggish because input polling was coupled to the display refresh rate.
**Action:** Decouple display logic from input logic. Use a non-blocking timer for the display (e.g., 10FPS) and let the main loop run as fast as possible for inputs.
