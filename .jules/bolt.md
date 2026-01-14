## 2024-05-22 - OLED I2C Blocking Bottleneck
**Learning:** Sending the full 1KB frame buffer to the SSD1306 OLED via I2C at 400kHz takes approximately 25ms. Doing this unconditionally in the main loop caps the loop frequency at ~40Hz and wastes CPU time.
**Action:** Implement a 'Dirty Flag' pattern (displayNeedsUpdate) to only transmit the buffer when visual data actually changes (sensors, user input, animations).
