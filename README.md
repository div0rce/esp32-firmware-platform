# ESP32 Firmware Platform

[![PlatformIO CI](https://github.com/div0rce/esp32-firmware-platform/actions/workflows/platformio.yml/badge.svg)](https://github.com/div0rce/esp32-firmware-platform/actions/workflows/platformio.yml)

## Summary

This repository demonstrates an ESP-IDF firmware platform for the Seeed Studio
XIAO ESP32S3 with both FreeRTOS and cooperative execution backends. Application
behavior is isolated behind a platform HAL and validated through host-native
tests, fault-injection coverage, scheduler timing tests, CI backend builds, a
Wokwi runtime gate, and bounded existing bring-up evidence.

ESP-IDF remains responsible for startup, system services, and hardware drivers.
The portable app core is designed so scheduler policy can move between the RTOS
backend and the cooperative backend without pulling ESP-IDF, FreeRTOS, or
Arduino headers into application behavior.

## What This Demonstrates

- ESP-IDF component layout for an ESP32S3 firmware target.
- FreeRTOS task, queue, timer, and watchdog backend.
- Cooperative single-loop backend using the same portable app core.
- HAL-isolated application logic through `components/app_core/include/platform_hal.h`.
- ADC sampling, range checking, hysteresis, and fault latching.
- GPIO button recovery path for recoverable fault state.
- UART telemetry formatting for samples, faults, runtime health, and boot self-test results.
- Host-native Unity tests for sensor math, state transitions, telemetry formatting, time rollover, app-core behavior, and scheduler timing.
- Host-side fake HAL tests for time, ADC, UART, GPIO, button, trace, reset reason, and watchdog observability.
- CI-enforced app-core include boundary.
- Bounded existing ESP32S3 serial and oscilloscope bench artifacts retained as historical evidence.

## Architecture

```text
main/app_main.cpp
  -> components/app_backends/
      -> components/app_core/
          -> components/app_core/include/platform_hal.h
              -> components/platform_esp32s3/
```

| Layer | Location | Responsibility |
|---|---|---|
| ESP-IDF entry point | `main/app_main.cpp` | Selects one execution backend from Kconfig |
| Execution backends | `components/app_backends/` | Own scheduler policy, RTOS integration, cooperative loop, queues, tasks, timers, and watchdog service points |
| Portable app core | `components/app_core/` | Owns cooperative sampling cadence, fault handling, telemetry emission, trace toggles, and HAL-driven behavior |
| Platform HAL contract | `components/app_core/include/platform_hal.h` | Defines the app-core boundary for ADC, UART, GPIO, button, time, trace, reset reason, and watchdog operations |
| ESP32S3 platform implementation | `components/platform_esp32s3/` | Implements the HAL using ESP-IDF ADC, GPIO, UART, timer, reset reason, and watchdog facilities |
| Portable legacy modules | `src/*.c`, `include/*.h` | Provide shared state machine, sensor math, telemetry formatting, and time helpers used by native tests and firmware paths |

The app-core boundary is enforced by `scripts/check_app_core_includes.sh`, which
blocks Arduino, FreeRTOS, ESP-IDF driver, timer, watchdog, log, and error headers
from the portable app-core paths.

## Execution Backends

The ESP-IDF entry point selects exactly one backend through
`main/Kconfig.projbuild`.

| Backend | Selection | Purpose | CI Evidence |
|---|---|---|---|
| RTOS | `CONFIG_APP_EXECUTION_MODEL_RTOS=y` | Preserves the FreeRTOS task, queue, timer, and watchdog runtime model | `esp-idf-builds (rtos)` |
| Cooperative | `CONFIG_APP_EXECUTION_MODEL_COOPERATIVE=y` | Runs shared application behavior from one explicit loop using `app_core_tick()` | `esp-idf-builds (cooperative)` |

CI config fragments build both variants:

- `sdkconfig.ci.rtos`
- `sdkconfig.ci.cooperative`

## Validation

| Validation Target | Evidence | Boundary |
|---|---|---|
| ESP-IDF RTOS backend build | `esp-idf-builds (rtos)` and local `idf.py -B build-rtos ... build` | Build validation only |
| ESP-IDF cooperative backend build | `esp-idf-builds (cooperative)` and local `idf.py -B build-cooperative ... build` | Build validation only |
| XIAO ESP32S3 PlatformIO build | `firmware-build` and `platformio run -e seeed_xiao_esp32s3` | Compile validation only |
| Host-native tests | `native-tests` and `platformio test -e native` | Host-side portable logic and fake-HAL behavior |
| App-core include boundary | `scripts/check_app_core_includes.sh` | Enforces listed include paths only |
| Wokwi runtime telemetry gate | `wokwi-runtime` and `scripts/wokwi_runtime_check.sh` | Simulator validation only |
| Fault-injection behavior | `docs/fault_injection_matrix.md` plus native tests and existing artifacts | Documents supported fault paths only |
| Claim-to-validation mapping | `docs/ci_evidence_map.md` | Maps current CI jobs and tests to bounded claims |
| Existing ESP32S3 bring-up logs | `docs/hardware_bringup.md`, `docs/hardware_fault_injection.md`, `docs/hardware_oscilloscope_trace.md`, and `artifacts/hardware/` | Historical bench evidence only; no new hardware evidence is added by this README |

## Evidence Boundaries

This project does not claim pure from-reset bare-metal firmware, hard real-time
determinism, automotive safety certification, production manufacturing
validation, or exhaustive hardware validation. ESP-IDF remains responsible for
startup, system services, and hardware drivers.

Existing ESP32S3 bring-up logs and captures are retained as bounded historical
evidence. They do not prove physical ADC accuracy, electrical safety, sensor
calibration, board-level correctness, field reliability, or product
qualification.

The cooperative backend demonstrates that the app behavior can run without
FreeRTOS tasks inside the app core. It does not replace ESP-IDF startup,
drivers, or platform services.

## Repository Layout

```text
components/app_core/          Portable app core and HAL contract
components/app_backends/      RTOS and cooperative execution backends
components/platform_esp32s3/  ESP-IDF platform HAL implementation
main/                         ESP-IDF application entry point
src/                          Portable C modules and legacy compatibility path
include/                      Shared public headers
test/                         Host-native Unity tests and fake HAL
docs/                         Architecture, timing, validation, and evidence notes
scripts/                      CI and local validation helpers
.github/workflows/            CI workflow definitions
platformio.ini                PlatformIO firmware, simulation, and native-test environments
CMakeLists.txt                ESP-IDF project definition
```

Key documentation:

- `docs/architecture.md`
- `docs/design.md`
- `docs/hal_boundary.md`
- `docs/timing_model.md`
- `docs/fault_injection_matrix.md`
- `docs/ci_evidence_map.md`
- `docs/toolchain.md`
- `docs/bare_metal_porting_notes.md`
- `docs/wiring.md`

## Build and Test

ESP-IDF is pinned to `v5.2.5`; see `docs/toolchain.md`.

```bash
# RTOS backend
idf.py -B build-rtos -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.rtos" set-target esp32s3 build

# Cooperative backend
idf.py -B build-cooperative -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.cooperative" set-target esp32s3 build

# Host-native behavioral tests
platformio test -e native

# HAL boundary guard
scripts/check_app_core_includes.sh

# Wokwi runtime gate, requires WOKWI_CLI_TOKEN
scripts/wokwi_runtime_check.sh
```

PlatformIO remains in the repository for host-native Unity tests, the current
Wokwi runtime check, the XIAO ESP32S3 firmware build path, and legacy Arduino
compatibility paths:

```bash
platformio run -e seeed_xiao_esp32s3
```
