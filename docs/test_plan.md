# Test Plan

## Required Gates

| Gate | Command | Pass condition |
|---|---|---|
| ESP-IDF RTOS build | `idf.py -B build-rtos -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.rtos" set-target esp32s3 build` | ESP32S3 image builds |
| ESP-IDF cooperative build | `idf.py -B build-cooperative -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.cooperative" set-target esp32s3 build` | ESP32S3 image builds |
| Host/native tests | `pio test -e native` | Existing Unity tests pass |
| HAL include guard | `scripts/check_app_core_includes.sh` | No forbidden runtime headers in app core |
| Legacy PlatformIO build | `pio run -e seeed_xiao_esp32s3` | Transitional Arduino build still compiles |

## Optional Hardware Gates

| Gate | Required? | Pass condition |
|---|---:|---|
| Serial sanity log | If board connected | Boot, telemetry output, and no observed watchdog reset |
| Physical long-run test | No | Explicitly deferred |
| JTAG / logic analyzer | No | Explicitly unavailable with current hardware |

## CI

GitHub Actions runs:

- `esp-idf-builds` for RTOS and cooperative variants
- `firmware-build`
- `native-tests`
- `wokwi-runtime` when `WOKWI_CLI_TOKEN` is available

The ESP-IDF CI job installs ESP-IDF `v5.2.5`, checks the app-core include boundary, and builds each execution model with its CI config fragment.
