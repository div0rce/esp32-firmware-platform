# Architecture

## Task Model

| Task | Purpose |
|---|---|
| `sensor_task` | Waits for timer requests, reads ADC, validates samples, sends telemetry payloads |
| `telemetry_task` | Owns UART output and prints formatted telemetry packets |
| `button_task` | Consumes GPIO button events and applies debounce in task context |
| `fault_task` | Tracks queue overflows, stale samples, and LED fault indication |

## Queues

| Queue | Producer | Consumer | Payload |
|---|---|---|---|
| `sample_request_queue` | ESP timer callback | `sensor_task` | `SampleRequest` |
| `sensor_sample_queue` | `sensor_task` | `telemetry_task` | `SensorSample` |
| `button_event_queue` | GPIO ISR | `button_task` | `ButtonEvent` |

## State Model

Normal path:

```text
BOOT -> IDLE -> SAMPLE -> TRANSMIT -> IDLE
```

Fault path:

```text
ANY_STATE -> FAULT -> RECOVERY -> IDLE
```

## Watchdog Layers

- ESP task watchdog: detects stuck registered FreeRTOS tasks.
- Application watchdog: detects stale sampling and enters recoverable `WATCHDOG_TIMEOUT`.

Task watchdog behavior is compiled and architecturally integrated. Physical watchdog reset behavior is not claimed without real hardware.

## Portability Boundary

The firmware separates portable C logic from ESP32/Arduino integration code.

Portable C modules:

- `sensor_math.c`
- `fault_state.c`
- `state_machine.c`
- `telemetry_format.c`
- `time_utils.c`

ESP32/Arduino integration modules:

- `main.cpp`
- `sensor.cpp`
- `telemetry.cpp`
- `watchdog.cpp`
- `sampling_timer.cpp`
- `button.cpp`

The native test environment validates the portable C layer without Arduino, ESP32 hardware, or vendor HAL dependencies.
