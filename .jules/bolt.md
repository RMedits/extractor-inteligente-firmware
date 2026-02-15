## 2026-02-15 - Unthrottled OLED Rendering
**Learning:** Found `drawScreen()` called in every loop iteration (approx 50Hz). Sending 128x64 buffer over I2C (100kHz) takes ~92ms, meaning the loop was effectively blocked by the display refresh, reducing the loop frequency drastically and wasting >80% of CPU time.
**Action:** Always check `loop()` for heavy I2C operations running unthrottled. Throttle display updates to human-perceptible rates (5-10Hz) to free up the main loop for sensors and logic.

## 2026-02-15 - GitHub Actions Missing Secrets
**Learning:** The `gemini-review` workflow failed because it attempted to run without necessary API keys (`GEMINI_API_KEY`, etc.), which are not present in all environments (e.g., forks).
**Action:** Always add a job-level `if` condition (e.g., `if: ${{ vars.GCP_WIF_PROVIDER || secrets.GEMINI_API_KEY || secrets.GOOGLE_API_KEY }}`) to workflows that depend on secrets, ensuring they skip gracefully instead of failing.
