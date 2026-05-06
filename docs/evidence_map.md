# Embedded Evidence Map

| Skill area | Evidence | Status |
|---|---|---:|
| ESP-IDF project structure | Root `CMakeLists.txt`, `main/`, `components/`, `sdkconfig.defaults` | Implemented |
| RTOS firmware | `components/app_backends/src/app_rtos_backend.cpp` | Builds |
| Bare-metal-compatible execution | `components/app_backends/src/app_cooperative_backend.cpp`, `components/app_core/src/app_core.cpp` | Builds |
| HAL isolation | `components/app_core/include/platform_hal.h`, `components/platform_esp32s3/` | Implemented |
| Runtime include guard | `scripts/check_app_core_includes.sh` | Passing |
| Native tests | `pio test -e native` | 31/31 passing locally |
| ESP-IDF RTOS build | `idf.py -B build-rtos -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.rtos" set-target esp32s3 build` | Passing locally |
| ESP-IDF cooperative build | `idf.py -B build-cooperative -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.cooperative" set-target esp32s3 build` | Passing locally |
| Legacy PlatformIO build | `pio run -e seeed_xiao_esp32s3` | Passing locally |
| Physical serial bring-up | `docs/hardware_bringup.md`, `artifacts/hardware/esp32_serial_log.txt` | Captured |
| Physical ADC fault injection | `docs/hardware_fault_injection.md`, `artifacts/hardware/esp32_fault_injection_log.txt` | Captured |
| GPIO timing trace | `docs/hardware_oscilloscope_trace.md`, `artifacts/hardware/oscilloscope_trace_midpoint.png` | Captured |
| JTAG / logic analyzer | Not available with current hardware | Unavailable |
| Physical long-run stability | Explicitly deferred | Deferred |

## Public Claim

Use this exact claim:

```text
ESP-IDF bare-metal-compatible port with RTOS and cooperative execution backends.
```

Do not describe this repository as pure bare-metal ESP32 firmware.
