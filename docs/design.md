# Design Notes

## Design Goal

This project is a small ESP32 firmware architecture prototype for a fault-monitored sensor node. It is meant to demonstrate modular embedded structure, timer-driven sampling, ISR-safe event handling, fault-state behavior, UART telemetry, watchdog logic, native tests, and CI.

It is not presented as production firmware or physical hardware bring-up.

## Module Boundaries

Hardware-facing modules stay in C++ because they depend on Arduino-ESP32, FreeRTOS, or ESP-IDF APIs:

| Module | Boundary |
|---|---|
| `main.cpp` | FreeRTOS task integration and queue ownership |
| `sensor.cpp` | `analogRead()` wrapper and sensor filter instance |
| `telemetry.cpp` | `Serial` ownership |
| `button.cpp` | GPIO interrupt event queue |
| `sampling_timer.cpp` | ESP timer setup and queue posting |
| `watchdog.cpp` | ESP task watchdog wrapper |
| `app_state.cpp` | FreeRTOS mutex-protected application state |

Pure logic modules are C-compatible so they can be tested without an ESP32 board:

| Module | Responsibility |
|---|---|
| `fault_state.c` | State and fault string conversion |
| `sensor_math.c` | ADC conversion, moving average, fault hysteresis |
| `telemetry_format.c` | Serial packet formatting |
| `state_machine.c` | Explicit state transition policy |
| `time_utils.c` | Unsigned elapsed-time checks |

## Timing Model

Sampling is triggered by an ESP timer callback running in ESP timer task context. The callback does not read ADC, print, allocate memory, or mutate application state. It posts a small `SampleRequest` to `sensor_task`.

`sensor_task` owns ADC reads. `telemetry_task` owns UART output. `button_task` performs debounce outside the GPIO ISR. `fault_task` checks queue overflow flags and stale-sample timing.

Unsigned 32-bit elapsed-time checks are isolated in `time_utils.c` and tested for rollover behavior.

## Fault Model

Faults are explicit enum values rather than string-only conditions:

| Fault | Source |
|---|---|
| `ADC_OUT_OF_RANGE` | Filtered ADC value enters low/high fault region |
| `SENSOR_QUEUE_SEND_FAILED` | Sensor sample queue is full |
| `BUTTON_QUEUE_SEND_FAILED` | Button ISR event queue overflows |
| `SAMPLE_TIMER_FAILED` | Sampling timer cannot post requests |
| `WATCHDOG_TIMEOUT` | Application watchdog detects stale sampling |

ADC fault classification uses a fixed-window moving average and separate enter/clear thresholds so readings near the boundary do not flicker between normal and fault states.

## Test Strategy

Native tests cover logic that does not require Arduino, FreeRTOS, or ESP-IDF:

- state and fault string conversion
- ADC conversion and fault classification
- ADC moving average and hysteresis boundaries
- telemetry packet formatting and small-buffer rejection
- state-machine transitions
- unsigned 32-bit elapsed-time rollover

The ESP32 build verifies that the Arduino/FreeRTOS integration compiles against the pinned PlatformIO `espressif32 @ 6.13.0` environment.

## Limitations

- No physical ESP32 board run is claimed.
- No oscilloscope or logic analyzer measurements are included.
- Wokwi simulation is configured, but serial output is not claimed until a real simulation artifact is captured.
- Arduino-ESP32 provides the hardware abstraction layer; this is not bare-metal register-level firmware.
- ESP task watchdog integration is compiled, but physical watchdog reset behavior is not validated without hardware.
- ADC behavior is architecturally modeled and unit-tested; actual ESP32 ADC calibration and noise behavior require board-level validation.

## Future Hardware Validation

When a board is available, validation should add real artifacts only:

- serial monitor capture from an ESP32 run
- wiring photo or clear simulation-labeled Wokwi screenshot
- measured sampling cadence
- button debounce observations
- fault recovery notes
- optional oscilloscope or logic analyzer trace
