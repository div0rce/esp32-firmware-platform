# ESP32 Fault-Monitored Sensor Node

[![PlatformIO CI](https://github.com/div0rce/esp32-firmware-platform/actions/workflows/platformio.yml/badge.svg)](https://github.com/div0rce/esp32-firmware-platform/actions/workflows/platformio.yml)

ESP-IDF bare-metal-compatible firmware port for a fault-monitored sensor node on the Seeed Studio XIAO ESP32S3. The firmware keeps a FreeRTOS task/queue backend and adds a cooperative single-loop backend behind the same portable application core.

This is not a pure from-reset, custom-linker, register-only ESP32-S3 firmware project. It demonstrates application-level scheduling discipline, HAL isolation, watchdog behavior, and measured/buildable timing on available hardware.

## What It Does

- Samples an analog input through the ESP32 ADC.
- Converts raw ADC counts into millivolts.
- Classifies invalid readings as explicit fault states.
- Handles a GPIO button recovery path.
- Emits compact UART telemetry packets.
- Preserves the original FreeRTOS task/queue execution model.
- Adds a cooperative execution backend where application behavior runs from a deterministic single-loop scheduler.
- Keeps portable logic out of Arduino, FreeRTOS, and ESP-IDF driver headers through an enforced HAL boundary.

## Execution Models

The ESP-IDF entry point selects exactly one backend through `main/Kconfig.projbuild`.

| Backend | Selection | Purpose |
|---|---|---|
| RTOS | `CONFIG_APP_EXECUTION_MODEL_RTOS=y` | Preserves the FreeRTOS task, queue, timer, and watchdog runtime model |
| Cooperative | `CONFIG_APP_EXECUTION_MODEL_COOPERATIVE=y` | Runs shared application behavior from one explicit loop using `app_core_tick()` |

CI config fragments are provided only to build both variants:

- `sdkconfig.ci.rtos`
- `sdkconfig.ci.cooperative`

## Architecture

| Layer | Location | Responsibility |
|---|---|---|
| Project entry | `main/app_main.cpp` | Selects RTOS or cooperative backend |
| Backends | `components/app_backends/` | Own scheduler policy and runtime integration |
| Portable app core | `components/app_core/` | Owns cooperative state, sampling cadence, fault handling, and telemetry formatting |
| Platform implementation | `components/platform_esp32s3/` | Owns ESP-IDF ADC, GPIO, time, UART, watchdog, and reset reason integration |
| Legacy portable modules | `src/*.c`, `include/*.h` | State machine, ADC math, telemetry formatting, timing helpers |

The include guard in `scripts/check_app_core_includes.sh` prevents runtime headers from leaking into portable firmware logic.

## Hardware Wiring

Configured pin map:

| Signal | XIAO ESP32S3 Pin |
|---|---:|
| ADC input | D2 / GPIO3 |
| Button input | D1 / GPIO2 |
| Status LED | D10 / GPIO9 |
| Sample trace, optional | D3 / GPIO4 |
| Telemetry trace, optional | D4 / GPIO5 |
| Fault trace, optional | D5 / GPIO6 |
| UART | USB serial, 115200 baud |

Physical wiring is documented in `docs/wiring.md`. The Wokwi circuit in `diagram.json` uses the generic `esp32dev` simulation pin map and is not the source of truth for the XIAO ESP32S3 wiring.

## Build And Test

ESP-IDF is pinned to `v5.2.5`; see `docs/toolchain.md`.

```bash
# RTOS backend
idf.py -B build-rtos -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.rtos" set-target esp32s3 build

# Cooperative backend
idf.py -B build-cooperative -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.cooperative" set-target esp32s3 build

# Host/native behavioral tests
pio test -e native

# HAL boundary guard
./scripts/check_app_core_includes.sh
```

PlatformIO remains only as the current host/native test path and legacy Arduino build path until replaced by CMake/CTest or ESP-IDF host testing:

```bash
pio run -e seeed_xiao_esp32s3
```

## Upload And Monitor

Requires a connected XIAO ESP32S3 board:

```bash
idf.py -B build-rtos -p PORT flash monitor
```

## Telemetry Format

Example packet format:

```text
timestamp_ms=12500,state=SAMPLE,adc_raw=1842,voltage_mv=1485,fault=NONE
timestamp_ms=12700,event=FAULT_EVENT,fault=ADC_OUT_OF_RANGE
```

## Embedded Evidence Map

| Embedded skill | Repository evidence | Status |
|---|---|---:|
| ESP-IDF firmware shell | Root `CMakeLists.txt`, `main/app_main.cpp`, ESP-IDF component layout | Verified by local build |
| RTOS firmware structure | `components/app_backends/src/app_rtos_backend.cpp` preserves FreeRTOS tasks and queues | Verified by local build |
| Bare-metal-compatible scheduling | `components/app_backends/src/app_cooperative_backend.cpp` runs shared logic through one cooperative loop | Verified by local build |
| HAL isolation | `components/app_core/include/platform_hal.h` plus `components/platform_esp32s3/` implementation | Verified by include guard |
| Forbidden runtime dependency guard | `scripts/check_app_core_includes.sh` blocks Arduino, FreeRTOS, ESP-IDF driver/log/error headers in app core | Verified locally and in CI |
| Developer-driven testing | PlatformIO native Unity tests cover state, timing, telemetry, ADC math, and integration behavior | 31/31 passing |
| Hardware bring-up | Physical XIAO ESP32S3 serial bring-up notes and committed serial excerpt | Captured |
| Fault injection and recovery | ADC low/mid/high physical fault-injection evidence plus native tests for hysteresis and recovery | Captured/tested |
| Hardware debugging | UART telemetry, optional GPIO trace pins, and selected oscilloscope timing screenshot | Captured |
| Size evidence | ESP-IDF build output reports image size for both RTOS and cooperative variants | Measured locally |

## Evidence Boundaries

This repository does not claim deterministic hard real-time behavior under all ESP-IDF system conditions. It demonstrates application-level scheduling discipline, HAL isolation, watchdog behavior, and measured timing/build evidence on available hardware.

It also does not claim physical ADC accuracy, electrical safety validation, sensor calibration, board-level correctness, production readiness, field reliability, a custom bootloader, a custom linker script, JTAG evidence, or logic-analyzer evidence.

## Documentation

- `docs/toolchain.md`
- `docs/architecture.md`
- `docs/design.md`
- `docs/hal_boundary.md`
- `docs/bare_metal_porting_notes.md`
- `docs/evidence_map.md`
- `docs/branch_protection.md`
- `docs/test_plan.md`
- `docs/validation_log.md`
- `docs/wiring.md`
- `docs/hardware_bringup.md`
- `docs/hardware_fault_injection.md`
- `docs/hardware_oscilloscope_trace.md`
