## 2024-05-23 - Throttling I2C Operations
**Learning:** In embedded loops with `delay()`, heavy I2C operations like `display.display()` and sensor reads can significantly extend the loop cycle, making the system sluggish and the delay unpredictable.
**Action:** Always throttle display updates and sensor polling to the minimum necessary frequency (e.g., 4Hz for UI, 0.5Hz for sensors) using non-blocking timers (`millis()`), rather than running them on every loop iteration.
