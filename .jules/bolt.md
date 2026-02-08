## 2024-05-24 - Sensor Throttling & Missing Fan Logic
**Learning:** The UI loop was coupled to the sensor reading frequency (200ms), causing significant lag due to the blocking 80ms delay in `Adafruit_AHTX0::getEvent()`. Throttling sensors to 2000ms liberated the UI loop.
**Action:** Always decouple slow I/O (like I2C sensors) from the main UI loop using non-blocking timers.

**Learning:** Despite the project being a "Smart Extractor", the code for `PWM_FAN_PIN` control is missing. The variable `ventiladorActivo` toggles the LED but never writes to the fan pin.
**Action:** Document this architectural gap; future tasks should address the missing fan control logic.
