# Validation Log

| Claim | Status | Evidence |
|---|---:|---|
| PlatformIO ESP32 firmware builds | Verified | `pio run -e esp32dev`, GitHub Actions |
| Native logic tests pass | Verified | `pio test -e native`, GitHub Actions |
| Wokwi simulation configured | Ready | `wokwi.toml`, `diagram.json` |
| Wokwi serial telemetry observed | Pending | No captured simulation output yet |
| Physical ESP32 hardware validation | Not available | No board |
| Oscilloscope / logic analyzer traces | Not available | No board |
| Real hardware bring-up | Not claimed | Correctly withheld |

Current validation is build, host-side logic tests, and Wokwi simulation readiness.

Physical ESP32 hardware validation is not claimed because no board was available during development.
