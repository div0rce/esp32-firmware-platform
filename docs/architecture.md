# Architecture

## ESP-IDF Component Model

| Component | Purpose |
|---|---|
| `main` | ESP-IDF entry point and execution-model selection |
| `components/app_backends` | RTOS and cooperative scheduler backends |
| `components/app_core` | Portable application behavior and HAL contract |
| `components/platform_esp32s3` | ESP32S3 hardware/runtime implementation |

## Execution Model Selection

`main/Kconfig.projbuild` defines one project-level choice:

```text
APP_EXECUTION_MODEL_RTOS
APP_EXECUTION_MODEL_COOPERATIVE
```

`main/app_main.cpp` dispatches to `app_rtos_start()` or `app_cooperative_start()`.

## RTOS Backend

The RTOS backend preserves the original task/queue design.

| Task | Purpose |
|---|---|
| `sensor_task` | Waits for timer requests, reads ADC, validates samples, sends telemetry payloads |
| `telemetry_task` | Owns UART output and prints formatted telemetry packets |
| `button_task` | Consumes GPIO button events and applies debounce in task context |
| `fault_task` | Tracks queue overflows, stale samples, watchdog faults, and LED indication |

| Queue | Producer | Consumer | Payload |
|---|---|---|---|
| `sample_request_queue` | ESP timer callback | `sensor_task` | `SampleRequest` |
| `sensor_sample_queue` | `sensor_task` | `telemetry_task` | `SensorSample` |
| `button_event_queue` | GPIO ISR | `button_task` | `ButtonEvent` |

## Cooperative Backend

The cooperative backend initializes the platform once, then repeatedly calls:

```c
app_core_tick(&context, platform_time_now_ms());
```

The loop owns scheduler cadence, trace toggles, watchdog kicks, and sleep policy. The app core owns sample scheduling, fault handling, telemetry formatting, and safe fallbacks for HAL failures.

## State Model

Normal path:

```text
BOOT -> IDLE -> SAMPLE -> TRANSMIT -> IDLE
```

Fault path:

```text
ANY_STATE -> FAULT -> RECOVERY -> IDLE
```

## Portability Boundary

`components/app_core` must not include Arduino, FreeRTOS, ESP-IDF driver, timer, watchdog, log, or error headers. That boundary is enforced by `scripts/check_app_core_includes.sh`.

Portable logic modules:

- `fault_state.c`
- `sensor_math.c`
- `state_machine.c`
- `telemetry_format.c`
- `time_utils.c`
- `components/app_core/src/app_core.cpp`

Platform/runtime integration modules:

- `components/platform_esp32s3/src/firmware_hal_espidf.cpp`
- `components/platform_esp32s3/src/platform_hal_esp32s3.cpp`
- `components/app_backends/src/app_rtos_backend.cpp`
- `components/app_backends/src/app_cooperative_backend.cpp`
