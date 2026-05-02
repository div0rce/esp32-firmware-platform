# Performance And Optimization Evidence

## Scope

This document records current firmware size, timing configuration, instrumentation overhead, and design trade-offs for the ESP32 fault-monitored sensor node.

The measurements here are build and configuration evidence from the current repository state. They are not physical timing closure, electrical validation, power profiling, stack high-water measurement, or production performance certification.

## Commands

```bash
/tmp/esp32-pio-venv/bin/pio run -e seeed_xiao_esp32s3
/tmp/esp32-pio-venv/bin/pio run -e seeed_xiao_esp32s3_trace
/tmp/esp32-pio-venv/bin/pio run -e esp32dev
/tmp/esp32-pio-venv/bin/pio test -e native
```

## Firmware Size Snapshot

Captured from PlatformIO build output.

| Environment | Purpose | RAM used | RAM capacity | RAM use | Flash used | Flash capacity | Flash use |
|---|---|---:|---:|---:|---:|---:|---:|
| `seeed_xiao_esp32s3` | Physical XIAO target | 19,460 B | 327,680 B | 5.9% | 266,989 B | 3,342,336 B | 8.0% |
| `seeed_xiao_esp32s3_trace` | Physical XIAO target with GPIO trace pins | 19,460 B | 327,680 B | 5.9% | 267,077 B | 3,342,336 B | 8.0% |
| `esp32dev` | Wokwi runtime target | 22,208 B | 327,680 B | 6.8% | 280,361 B | 1,310,720 B | 21.4% |

## Trace Instrumentation Overhead

Compared with the default XIAO build:

| Build comparison | RAM delta | Flash delta | Interpretation |
|---|---:|---:|---|
| `seeed_xiao_esp32s3_trace` vs `seeed_xiao_esp32s3` | 0 B | +88 B | Optional trace pins add negligible code size and no reported static RAM cost |

The trace build is intentionally separate from the default build through `ENABLE_TRACE_PINS`, so normal firmware does not toggle GPIO trace pins.

## Timing Configuration

These values are configured firmware targets.

| Parameter | Value | Rationale |
|---|---:|---|
| `SAMPLE_PERIOD_MS` | 100 ms | Periodic ADC sampling without excessive UART traffic |
| `TELEMETRY_PERIOD_MS` | 250 ms | Bounded telemetry task receive timeout |
| `FAULT_TASK_PERIOD_MS` | 50 ms | Frequent overflow and stale-sample checks relative to the app watchdog |
| `APP_WATCHDOG_TIMEOUT_MS` | 1000 ms | Detect stale sampling while avoiding false trips from normal scheduling jitter |
| `BUTTON_DEBOUNCE_MS` | 200 ms | Reject repeated button edges from mechanical bounce |
| `TASK_WATCHDOG_TIMEOUT_SECONDS` | 3 s | ESP task watchdog guard for registered FreeRTOS tasks |

Physical oscilloscope evidence for selected GPIO trace behavior is documented in `docs/hardware_oscilloscope_trace.md`.

## Optimization Trade-Offs

| Decision | Benefit | Cost | Current assessment |
|---|---|---|---|
| Separate FreeRTOS tasks for sensor, telemetry, button, and fault logic | Clear ownership, isolated watchdog feeding, easier timing observation | More stack allocation than a single superloop | Acceptable for clarity and fault isolation |
| Queue-backed button ISR path | Keeps ISR short and avoids doing debounce in interrupt context | Requires queue capacity and overflow handling | Appropriate for robust embedded behavior |
| Text UART telemetry | Easy bench debugging, CI parsing, and artifact review | Slower and larger than binary telemetry | Acceptable for prototype validation and portfolio evidence |
| Fixed-window ADC moving average | Reduces threshold chatter near ADC fault limits | Adds filtering latency to fault clearing | Acceptable at 100 ms sample cadence |
| Optional GPIO trace pins | Enables oscilloscope timing visibility | Consumes GPIOs in trace builds | Good diagnostic trade-off because default build leaves trace disabled |
| Boot-time self-test telemetry | Gives bring-up/factory-style observability | Adds one ADC read and one boot UART line | Low overhead and useful for bench validation |
| Runtime health telemetry | Reports heap minimum, task stack high-water marks, task loop counters, and queue depth from `telemetry_task` | Adds one diagnostic UART line per second | Useful for bench validation without changing UART ownership |

## Current Evidence Gaps

The following are not measured in this repository yet:

- Queue high-water occupancy.
- Current draw or power consumption.
- Long-run physical stability duration beyond the committed short captures.
- Timing closure under worst-case interrupt or UART load.

## Boundary

This document is performance and optimization evidence for the current firmware prototype. It does not prove physical ADC accuracy, electrical safety, sensor calibration, board-level correctness, field reliability, production readiness, or timing closure.
