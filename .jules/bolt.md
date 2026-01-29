# Bolt's Journal

## 2026-01-29 - Decoupling Sensor I/O from UI Loop
**Learning:** The main loop architecture coupled heavy I2C sensor readings (AHT20/BMP280) to the display refresh rate (5Hz). This caused potential UI lag due to blocking I2C calls or measurement delays (e.g., AHT20 ~80ms).
**Action:** Decouple slow I/O operations into their own throttled timers (e.g., 2000ms) independent of the UI loop to ensure interface responsiveness.

## 2026-01-29 - GitHub Actions Secret Management
**Learning:** The `google-github-actions/run-gemini-cli` action fails with exit code 1 if no authentication secrets are provided, causing CI failures on forks.
**Action:** Always wrap the Gemini review step with an `if` condition checking for the existence of `vars.GCP_WIF_PROVIDER`, `secrets.GEMINI_API_KEY`, or `secrets.GOOGLE_API_KEY`.
