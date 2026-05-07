# CI Evidence Map

This map connects repository claims to concrete CI jobs, scripts, tests, or
local validation commands. It describes what the automated checks prove and
where their boundaries are.

| Claim | CI / Validation Evidence | Boundary |
|---|---|---|
| RTOS backend builds | `esp-idf-builds (rtos)` runs `idf.py -B build-rtos -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.rtos" set-target esp32s3 build`. | Build validation only; it does not prove runtime timing or physical behavior. |
| Cooperative backend builds | `esp-idf-builds (cooperative)` runs `idf.py -B build-cooperative -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.cooperative" set-target esp32s3 build`. | Build validation only; scheduler behavior is covered separately by host-native tests. |
| XIAO ESP32S3 PlatformIO firmware build remains intact | `firmware-build` runs `platformio run -e seeed_xiao_esp32s3`. | Confirms the PlatformIO firmware path compiles; it is not a hardware run. |
| Host-native unit tests pass | `native-tests` runs `platformio test -e native`. | Covers portable logic and host fake-HAL behavior, not ESP32S3 driver execution. |
| App core excludes Arduino, FreeRTOS, and ESP-IDF runtime headers | `scripts/check_app_core_includes.sh` runs in `esp-idf-builds`, `firmware-build`, and `native-tests`. | Enforces include boundaries for listed paths only. |
| Telemetry formatting is tested | `test/test_telemetry_format/test_telemetry_format.cpp` runs under `native-tests`. | Validates formatter output strings and buffer handling, not UART electrical signaling. |
| Fault strings are tested | `test/test_fault_state/test_fault_state.cpp` runs under `native-tests`. | Validates enum-to-string mapping, not complete system fault coverage by itself. |
| ADC math and hysteresis are tested | `test/test_sensor_math/test_sensor_math.cpp` runs under `native-tests`. | Validates portable math and thresholds, not physical ADC accuracy. |
| State machine transitions are tested | `test/test_state_machine/test_state_machine.cpp` and `test/test_integration/test_integration.cpp` run under `native-tests`. | Validates modeled transitions and integration behavior, not all possible runtime interleavings. |
| Rollover-safe time helper is tested | `test/test_time_utils/test_time_utils.cpp` and `test/test_scheduler_timing/test_scheduler_timing.cpp` run under `native-tests`. | Validates unsigned wraparound arithmetic in host tests, not oscillator accuracy. |
| Cooperative scheduler timing policy is tested | `test/test_scheduler_timing/test_scheduler_timing.cpp` runs under `native-tests`. | Validates application-level tick policy, not hard timing guarantees on ESP-IDF. |
| App-core behavior is testable without hardware | `test/test_app_core/test_app_core.cpp` uses `test/fakes/fake_platform_hal.cpp` under `native-tests`. | Validates behavior through the HAL contract, not platform driver implementation. |
| Wokwi simulator runtime telemetry gate passes | `wokwi-runtime` runs `scripts/wokwi_runtime_check.sh`, which builds the `esp32dev` ELF, runs Wokwi CLI, and parses serial telemetry with `scripts/parse_wokwi_telemetry.py`. | Simulator validation only; it requires the repository secret `WOKWI_CLI_TOKEN` and does not prove physical ADC accuracy or board-level correctness. |

## Boundary

CI proves the listed build, test, architecture-boundary, and simulator-runtime
checks. It does not convert simulator behavior or host-native tests into
physical calibration, electrical validation, field reliability, or product
qualification evidence.
