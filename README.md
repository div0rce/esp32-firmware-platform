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
- Includes bounded physical XIAO ESP32S3 serial evidence and oscilloscope GPIO trace evidence.

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

| Signal | XIAO ESP32S3 Pin |
|---|---:|
| ADC input | D2 / GPIO3 |
| Button input | D1 / GPIO2 |
| Status LED | D10 / GPIO9 |
| Sample trace, optional | D3 / GPIO4 |
| Telemetry trace, optional | D4 / GPIO5 |
| Fault-task trace, optional | D5 / GPIO6 |
| UART | USB serial, 115200 baud |

Physical wiring is documented in `docs/wiring.md`. The Wokwi circuit in `diagram.json` uses the generic `esp32dev` simulation pin map and is not the source of truth for the XIAO ESP32S3 wiring.

## Build And Test

```bash
pio run -e seeed_xiao_esp32s3
pio test -e native
```

## Upload And Monitor

Requires a connected XIAO ESP32S3 board:

```bash
pio run -e seeed_xiao_esp32s3 -t upload
pio device monitor
```

## Wokwi Simulation

Build the `esp32dev` simulation firmware first, then open the project with Wokwi using the root `wokwi.toml` and `diagram.json` files. CI also runs a bounded Wokwi runtime check that requires non-empty telemetry, `fault=NONE`, and no `task_wdt` reset.

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
| Wokwi runtime telemetry gate | Verified | `scripts/wokwi_runtime_check.sh`, GitHub Actions `wokwi-runtime` |
| Physical XIAO ESP32S3 serial bring-up | Verified | `docs/hardware_bringup.md`, `artifacts/hardware/esp32_serial_log.txt` |
| Physical ADC fault-injection behavior | Verified | `docs/hardware_fault_injection.md`, `artifacts/hardware/esp32_fault_injection_log.txt` |
| Oscilloscope GPIO timing trace | Captured | `docs/hardware_oscilloscope_trace.md`, `artifacts/hardware/oscilloscope_trace_midpoint.png` |
| Long-run physical stability | Pending | No 30-minute physical run artifact currently included |

Current validation includes CI build, host-side logic tests, a Wokwi runtime telemetry gate, bounded physical XIAO ESP32S3 serial evidence, physical ADC fault-injection evidence, and a selected oscilloscope GPIO timing trace.

## Embedded Evidence Map

| Embedded skill | Repository evidence |
|---|---|
| RTOS firmware structure | FreeRTOS `sensor_task`, `telemetry_task`, `button_task`, and `fault_task` with queue-backed ownership boundaries |
| ISR-safe event handling | GPIO button interrupt posts `ButtonEvent` payloads through a FreeRTOS queue and debounces in task context |
| Hardware bring-up | Physical XIAO ESP32S3 serial bring-up notes and committed serial excerpt in `artifacts/hardware/esp32_serial_log.txt` |
| Fault injection and recovery | ADC low/mid/high physical fault-injection evidence plus native tests for fault state behavior and ADC hysteresis |
| Developer-driven testing | PlatformIO native Unity tests, GitHub Actions firmware build, and Wokwi runtime telemetry gate |
| Performance trade-offs | Firmware size snapshot, timing configuration, trace overhead, and documented optimization decisions in `docs/performance_optimization_evidence.md` |
| Hardware debugging | UART telemetry, optional GPIO trace pins, and selected oscilloscope timing screenshot |
| Bare-metal awareness | Portable C logic boundary and explicit bare-metal porting notes in `docs/bare_metal_porting_notes.md` |
| Manufacturing-style diagnostics | Boot-time self-test telemetry and failure behavior documented in `docs/manufacturing_self_test.md` |

This map is intended as a quick reviewer guide. It points to implemented evidence and does not expand the claims beyond the validation boundaries below.

## Evidence Boundaries

This repository currently provides build validation, host-side native tests, Wokwi simulation runtime checks, physical serial bench evidence, and selected oscilloscope GPIO trace evidence.

It does not claim physical ADC accuracy, electrical safety validation, sensor calibration, board-level correctness, production readiness, field reliability, or timing closure. The oscilloscope evidence is a selected GPIO timing trace, not a logic analyzer capture.

## Limitations

- No 30-minute physical long-run stability artifact is currently included.
- No logic analyzer capture is included.
- This is an Arduino-ESP32 firmware prototype, not a bare-metal or production firmware stack.
- Physical serial captures show no observed `task_wdt` reset in the bounded runs, but this is not field reliability evidence.
- Physical ADC fault behavior is demonstrated by manual potentiometer movement, but physical ADC accuracy and calibration are not claimed.

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
- `docs/hardware_bringup.md`
- `docs/hardware_fault_injection.md`
- `docs/hardware_oscilloscope_trace.md`
