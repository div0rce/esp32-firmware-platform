# Validation Log

| Claim | Status | Evidence |
|---|---:|---|
| PlatformIO XIAO ESP32S3 firmware builds | Verified | `pio run -e seeed_xiao_esp32s3` |
| Native logic tests pass | Verified | `pio test -e native`, GitHub Actions |
| Wokwi simulation configured | Ready | `wokwi.toml`, `diagram.json`, `esp32dev` simulation pin map |
| Wokwi serial telemetry observed | Pending | No captured simulation output yet |
| Physical XIAO ESP32S3 hardware validation | Pending | Board wiring prepared; no run captured yet |
| Oscilloscope / logic analyzer traces | Not available | No trace captured |
| Real hardware bring-up | Not claimed | Correctly withheld |

Current validation is XIAO ESP32S3 build, generic ESP32 Wokwi-target build, host-side logic tests, and Wokwi simulation readiness.

Physical XIAO ESP32S3 hardware validation is not claimed until serial output from the wired board is captured.
