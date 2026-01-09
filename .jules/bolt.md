# Bolt's Performance Journal

## 2024-05-22 - Display Rendering Optimization
**Learning:** The OLED display was being refreshed in every iteration of the main loop (~50Hz), regardless of whether the content had changed. This consumed significant CPU time and I2C bandwidth.
**Action:** Implemented a 'dirty flag' (`displayNeedsUpdate`) pattern. The display is now only updated when a relevant state change occurs (sensor reading, user input, mode change, or timer update). This drastically reduces overhead.
