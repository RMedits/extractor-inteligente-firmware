## 2024-05-22 - [Decoupling Sensors from UI Loop]
**Learning:** Memory indicated that sensor readings were already decoupled and throttled to 2000ms, but code analysis revealed they were actually inside the 200ms UI loop. This caused the 80ms blocking delay of AHT20 sensor to stall the UI 5 times a second.
**Action:** Always verify "known" architectural facts against the actual code. Decoupling expensive I/O operations from high-frequency UI loops is a critical pattern for responsiveness in single-threaded embedded systems.
