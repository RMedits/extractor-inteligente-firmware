## 2024-05-23 - Missing Throttling in Main Loop
**Learning:** The codebase documentation/memory stated that I2C throttling was implemented, but the actual code executed heavy I2C operations (Display, Sensors) on every loop cycle with a blocking delay.
**Action:** Always verify that documented performance features are actually present in the code. Trust code over docs.
