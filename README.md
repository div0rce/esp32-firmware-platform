# ESP32 Fault-Monitored Sensor Node

[![PlatformIO CI](https://github.com/div0rce/esp32-firmware-platform/actions/workflows/platformio.yml/badge.svg)](https://github.com/div0rce/esp32-firmware-platform/actions/workflows/platformio.yml)

ESP32 firmware architecture with Wokwi configuration files present, targeting PlatformIO, Arduino-ESP32, FreeRTOS task design, native-tested firmware logic, and CI-backed validation.

The repository name remains `esp32-firmware-platform` for stable links. The project itself is a fault-monitored sensor node firmware prototype.

## What It Does

This firmware models a small embedded monitoring system:

- Samples an analog input through the ESP32 ADC.
- Converts raw ADC counts into millivolts.
- Classifies invalid readings as explicit fault states.
- Handles a GPIO button interrupt through a queue-backed event path.
- Emits compact UART telemetry packets.
- Uses FreeRTOS tasks and queues to separate sampling, telemetry, button handling, and fault monitoring.
- Integrates ESP task watchdog feeding plus an application watchdog for stale sampling.

## Architecture

The hardware-facing shell uses Arduino-ESP32 and FreeRTOS. Pure firmware logic is separated into modules that can be tested under PlatformIO's native environment without an ESP32 board.

Current task model:

| Task | Responsibility |
|---|---|
| `sensor_task` | Waits for timer requests, reads ADC, validates samples, queues telemetry |
| `telemetry_task` | Owns `Serial` output and prints formatted packets |
| `button_task` | Consumes button ISR events and applies debounce in task context |
| `fault_task` | Tracks queue overflow, stale samples, watchdog faults, and LED fault indication |

Current queue model:

| Queue | Producer | Consumer |
|---|---|---|
| `sample_request_queue` | ESP timer callback | `sensor_task` |
| `sensor_sample_queue` | `sensor_task` | `telemetry_task` |
| `button_event_queue` | GPIO ISR | `button_task` |

## Hardware Wiring

Configured pin map:

| Signal | XIAO ESP32C3 Pin |
|---|---:|
| ADC input | D2 / GPIO4 |
| Button input | D1 / GPIO3 |
| Status LED | D10 / GPIO10 |
| UART | USB serial, 115200 baud |

Physical wiring is documented in `docs/wiring.md`. The Wokwi circuit in `diagram.json` uses the generic `esp32dev` simulation pin map and is not the source of truth for the XIAO ESP32C3 wiring.

## Build And Test

```bash
pio run -e seeed_xiao_esp32c3
pio test -e native
```

## Upload And Monitor

Requires a connected XIAO ESP32C3 board:

```bash
pio run -e seeed_xiao_esp32c3 -t upload
pio device monitor
```

## Wokwi Simulation

Build the `esp32dev` simulation firmware first, then open the project with Wokwi using the root `wokwi.toml` and `diagram.json` files. Wokwi configuration files are present, but no Wokwi run artifact is currently included.

## Telemetry Format

Example packet format only; not captured run output:

```text
timestamp_ms=12500,state=SAMPLE,adc_raw=1842,voltage_mv=1485,fault=NONE
timestamp_ms=12700,event=FAULT_EVENT,fault=ADC_OUT_OF_RANGE
```

Pure logic modules are separated from Arduino-dependent modules so that conversion, fault detection, state naming, and telemetry formatting can be unit-tested under PlatformIO's native environment.

## Validation Status

| Claim | Status | Evidence |
|---|---:|---|
| PlatformIO ESP32 firmware builds | Verified | Local build + GitHub Actions |
| Native logic tests pass | Verified | Local tests + GitHub Actions |
| Wokwi configuration files present | Configured | `wokwi.toml`, `diagram.json` |
| Wokwi serial telemetry artifact | Pending | No Wokwi run artifact currently included |
| Physical ESP32 hardware validation | Not available | No board |
| Oscilloscope / logic analyzer traces | Not available | No board |
| Real hardware bring-up | Not claimed | Correctly withheld |

Current validation is build, host-side logic tests, and Wokwi configuration presence. Physical ESP32 hardware validation is not claimed because no board run has been captured.

## Evidence Boundaries

This repository currently provides build validation, host-side native tests, Wokwi configuration files, and design-level timing and porting notes.

It does not claim physical ESP32 hardware validation, measured timing traces, oscilloscope/logic analyzer evidence, or captured Wokwi serial output.

## Limitations

- No physical ESP32 board run has been captured.
- No oscilloscope or logic analyzer timing trace is included.
- Wokwi output has not yet been captured as a validation artifact.
- This is an Arduino-ESP32 firmware prototype, not a bare-metal or production firmware stack.
- Task watchdog integration is compiled and architecturally integrated; physical watchdog reset behavior is not claimed.

## Documentation

- `docs/design.md`
- `docs/architecture.md`
- [Failure-injection behavior](docs/failure_injection_behavior.md)
- [Timing budget](docs/timing_budget.md)
- [Bare-metal porting notes](docs/bare_metal_porting_notes.md)
- `docs/wiring.md`
- `docs/simulation_plan.md`
- `docs/test_plan.md`
- `docs/validation_log.md`
- `docs/timing_notes.md`
