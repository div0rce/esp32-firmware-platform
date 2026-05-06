# Bare-Metal Porting Notes

This project is now an ESP-IDF bare-metal-compatible port with RTOS and cooperative execution backends.

It is not pure register-only ESP32-S3 firmware. ESP-IDF still provides startup, system services, drivers, and FreeRTOS underneath the project. The cooperative backend demonstrates that application behavior does not require FreeRTOS tasks, FreeRTOS queues, Arduino APIs, or ESP-IDF driver headers inside the portable app core.

## Portable Logic

The following behavior remains portable C/C++ behind the HAL boundary:

- state and fault string conversion
- ADC scaling, moving average, and fault hysteresis
- explicit state-machine transitions
- telemetry packet formatting
- rollover-safe elapsed-time checks
- cooperative app-core tick scheduling

## Execution Backends

| Backend | File | Claim |
|---|---|---|
| RTOS | `components/app_backends/src/app_rtos_backend.cpp` | FreeRTOS task/queue firmware model preserved |
| Cooperative | `components/app_backends/src/app_cooperative_backend.cpp` | Shared application behavior runs from one explicit scheduler loop |

## Hardware Integration To Replace In A Lower-Level Port

| Current integration | Lower-level replacement |
|---|---|
| `platform_time_now_ms()` and `platform_time_sleep_ms()` | hardware timer tick and busy/low-power wait policy |
| `platform_uart_write_line()` | UART register driver |
| `platform_gpio_*()` and `platform_button_read()` | register-backed GPIO input/output and interrupt handling |
| `platform_adc_read_raw()` | MCU ADC register/HAL access |
| `platform_watchdog_kick()` | MCU watchdog register/service routine |
| ESP-IDF startup and partition flow | custom startup, linker, and flashing model |

The state machine, fault classification, telemetry formatting, ADC scaling, hysteresis policy, and rollover-safe timing logic should remain portable.

## Boundary

This repository does not claim deterministic hard real-time behavior under all ESP-IDF system conditions. It demonstrates application-level scheduling discipline, HAL isolation, watchdog behavior, and measured timing/build evidence on available hardware.
