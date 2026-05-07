# Cooperative Timing Model

This document describes the application-level timing model exercised by the host-native scheduler tests. It covers the portable cooperative app core and does not prove hard real-time execution, interrupt latency, or timing closure on ESP32S3 hardware.

## Timing Inputs

| Input | Source | Current value |
|---|---|---:|
| Sample period | `SAMPLE_PERIOD_MS` | 100 ms |
| Telemetry receive period | `TELEMETRY_PERIOD_MS` | 250 ms |
| Fault task period | `FAULT_TASK_PERIOD_MS` | 50 ms |
| App watchdog stale-sample timeout | `APP_WATCHDOG_TIMEOUT_MS` | 1000 ms |

## Cooperative Tick Invariant

`app_core_tick()` is called with a monotonic `uint32_t` timestamp. On each tick, the app core:

- services button recovery before sampling;
- emits at most one ADC sample if `now_ms` has reached `next_sample_ms`;
- schedules the next sample from the current tick time after a sample is taken;
- updates fault-check and watchdog-service bookkeeping every tick.

If several sample periods elapsed between two calls, the current policy is to take one sample on that tick and schedule the next sample at `now_ms + SAMPLE_PERIOD_MS`. The app core does not attempt to replay missed samples.

## Rollover Handling

The cooperative scheduler uses signed `uint32_t` delta comparison for sample due checks. Shared time helper tests cover rollover-safe elapsed-time behavior in `elapsed_ms_u32()`, and scheduler tests cover sampling at a deadline near `UINT32_MAX`.

## Evidence Boundary

The timing tests validate deterministic application-level scheduler behavior under host-native execution. They do not validate worst-case ESP-IDF scheduling latency, interrupt latency, oscillator accuracy, physical GPIO timing, or hard real-time behavior.
