# Design Notes

## Design Goal

This project demonstrates an ESP32S3 firmware architecture that separates portable application behavior from scheduler and hardware integration.

The public claim is:

```text
ESP-IDF bare-metal-compatible port with RTOS and cooperative execution backends.
```

It is not production firmware, pure register-only firmware, or a hard-real-time proof under all ESP-IDF system conditions.

## Module Boundaries

Hardware-facing modules stay outside `components/app_core` because they depend on ESP-IDF, FreeRTOS, or board-specific behavior:

| Module | Boundary |
|---|---|
| `main/app_main.cpp` | Project-level backend selection |
| `components/app_backends/src/app_rtos_backend.cpp` | FreeRTOS task, queue, timer, and watchdog integration |
| `components/app_backends/src/app_cooperative_backend.cpp` | Cooperative scheduler loop and watchdog kick cadence |
| `components/platform_esp32s3/src/platform_hal_esp32s3.cpp` | ESP32S3 platform HAL implementation |
| `components/platform_esp32s3/src/firmware_hal_espidf.cpp` | ESP-IDF ADC, GPIO, UART, timer, and interrupt wrappers |

Portable logic is C/C++ and built behind the HAL boundary:

| Module | Responsibility |
|---|---|
| `components/app_core/src/app_core.cpp` | Cooperative app behavior and failure fallback policy |
| `fault_state.c` | State and fault string conversion |
| `sensor_math.c` | ADC conversion, moving average, fault hysteresis |
| `telemetry_format.c` | Serial packet formatting |
| `state_machine.c` | Explicit state transition policy |
| `time_utils.c` | Unsigned elapsed-time checks |

## Timing Model

The RTOS backend uses ESP-IDF/FreeRTOS tasks and queues. Sampling is triggered by an ESP timer path and handled in task context.

The cooperative backend uses one loop with a 1 ms sleep policy. It calls `app_core_tick()` with monotonic platform time, then kicks the watchdog. This shows scheduler ownership can change without moving ESP-IDF driver headers into portable application behavior.

Unsigned 32-bit elapsed-time checks remain isolated and tested for rollover behavior.

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

## Failure Paths

HAL functions that can fail return `bool`. The cooperative app core treats ADC read failure as an explicit ADC fault, treats UART write failure as non-fatal telemetry loss, and holds in a safe sleep loop if platform initialization fails.

## Test Strategy

Required gates:

- ESP-IDF RTOS build
- ESP-IDF cooperative build
- PlatformIO native Unity tests
- PlatformIO legacy ESP32S3 build
- app-core include guard

Optional hardware evidence remains bounded to available tools: serial telemetry, ADC fault injection, and oscilloscope GPIO trace evidence. JTAG and logic-analyzer evidence are not claimed.
