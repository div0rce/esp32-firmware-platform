# Validation Log

| Claim | Status | Evidence |
|---|---:|---|
| ESP-IDF RTOS backend builds | Verified locally | `idf.py -B build-rtos-pr3 -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.rtos" set-target esp32s3 build` |
| ESP-IDF cooperative backend builds | Verified locally | `idf.py -B build-cooperative-pr3 -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.cooperative" set-target esp32s3 build` |
| App core excludes runtime/vendor headers | Verified locally | `scripts/check_app_core_includes.sh` |
| Native logic tests pass | Verified locally | `pio test -e native`, 31/31 passing |
| Legacy PlatformIO XIAO ESP32S3 firmware builds | Verified locally | `pio run -e seeed_xiao_esp32s3` |
| Wokwi simulation configured | Configured | `wokwi.toml`, `diagram.json`, `esp32dev` simulation pin map |
| Physical XIAO ESP32S3 serial bring-up | Captured | `docs/hardware_bringup.md`, `artifacts/hardware/esp32_serial_log.txt` |
| Physical ADC fault-injection behavior | Captured | `docs/hardware_fault_injection.md`, `artifacts/hardware/esp32_fault_injection_log.txt` |
| Oscilloscope GPIO timing trace | Captured | `docs/hardware_oscilloscope_trace.md`, `artifacts/hardware/oscilloscope_trace_midpoint.png` |
| JTAG / logic analyzer | Unavailable | No hardware available |
| Physical long-run stability | Deferred | Not part of current evidence set |

Current validation covers buildability, native behavior, HAL boundary enforcement, bounded physical serial evidence, physical ADC fault injection, and a selected oscilloscope GPIO trace.

This validation does not prove deterministic hard real-time behavior under all ESP-IDF system conditions, physical ADC accuracy, production readiness, field reliability, JTAG debugging, or logic-analyzer timing.
