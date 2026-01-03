## 2024-05-23 - Display Update Bottleneck
**Learning:** Sending a full 128x64 buffer to an OLED display via I2C every loop iteration consumes significant bandwidth (~25ms at 400kHz) and CPU time, slowing down input polling and other tasks.
**Action:** Always decouple display rendering from the main logic loop using a non-blocking timer (throttling), typically 10-20FPS is sufficient for UI, while logic/inputs should run as fast as possible.
