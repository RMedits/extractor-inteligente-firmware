## 2026-01-28 - Throttle Sensor Polling
**Learning:** Found critical discrepancy between memory ("sensors throttled") and code (sensors inside 200ms UI loop). The blocking `aht.getEvent()` (~80ms) was freezing the UI at 5Hz.
**Action:** Decoupled sensor polling to 2000ms independent timer. Always verify "known" optimizations in actual code.

## 2026-01-28 - CI Infrastructure Stability
**Learning:** `gemini-review` workflow fails hard when secrets are missing in forks/PRs, blocking unrelated checks.
**Action:** Always wrap API-dependent CI steps in `if: ${{ secrets.KEY != '' }}` or similar checks to ensure robustness.
