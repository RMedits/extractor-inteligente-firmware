## 2024-05-21 - Sensor Throttling Reality Gap
**Learning:** The memory stated sensor readings were throttled to 2000ms, but `src/main.cpp` revealed they were tightly coupled to the 200ms UI loop. Always trust the code over memory/docs.
**Action:** Decoupled sensors to dedicated 2000ms timer loop to unblock UI.
