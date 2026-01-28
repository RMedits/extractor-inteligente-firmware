## 2026-01-28 - Throttle Sensor Polling
**Learning:** Found critical discrepancy between memory ("sensors throttled") and code (sensors inside 200ms UI loop). The blocking `aht.getEvent()` (~80ms) was freezing the UI at 5Hz.
**Action:** Decoupled sensor polling to 2000ms independent timer. Always verify "known" optimizations in actual code.
