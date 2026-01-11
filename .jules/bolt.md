# Bolt's Journal - Critical Learnings

## 2024-05-22 - Initial Assessment
**Learning:** The application uses a classic "super loop" architecture with `display.display()` being called in every iteration of the loop (via `draw...` functions).
**Action:** This is a major bottleneck. The OLED update via I2C is slow. The first optimization should be to implement a "dirty flag" system to only update the display when data actually changes (sensor readings, mode change, user input).
