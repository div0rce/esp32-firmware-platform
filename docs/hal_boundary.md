# Hardware Abstraction Boundary

The ESP-IDF port separates portable application behavior from hardware/runtime integration through `components/app_core/include/platform_hal.h`.

## HAL Surface

```c
bool platform_init(void);
uint32_t platform_time_now_ms(void);
void platform_time_sleep_ms(uint32_t delay_ms);
bool platform_adc_read_raw(uint16_t *raw_out);
bool platform_uart_write_line(const char *line);
void platform_gpio_write(platform_gpio_id_t pin, bool value);
bool platform_gpio_read(platform_gpio_id_t pin);
bool platform_button_read(void);
void platform_trace_set(platform_trace_id_t trace, bool value);
void platform_watchdog_kick(void);
platform_reset_reason_t platform_reset_reason_get(void);
```

HAL functions that can fail return `bool`; application logic must define safe fallback behavior for failed ADC, UART, or platform initialization paths.

## Implemented Boundary

| Hardware concern | ESP-IDF implementation | Portable caller |
|---|---|---|
| Platform init | `components/platform_esp32s3/src/platform_hal_esp32s3.cpp` | `app_cooperative_backend.cpp` |
| UART output | `platform_uart_write_line()` through ESP-IDF stdio/UART shell | `app_core.cpp` |
| ADC input | `platform_adc_read_raw()` through ADC oneshot path | `app_core.cpp` |
| GPIO output/input | `platform_gpio_write()`, `platform_gpio_read()`, `platform_button_read()` | `app_core.cpp` |
| Trace pins | `platform_trace_set()` | cooperative loop and app core |
| Monotonic time/sleep | `platform_time_now_ms()`, `platform_time_sleep_ms()` | cooperative backend and app core |
| Task watchdog | `platform_watchdog_kick()` | cooperative loop |
| Reset diagnostics | `platform_reset_reason_get()` | available for bring-up and diagnostics |

## Guardrail

`scripts/check_app_core_includes.sh` fails the build if app-core code includes Arduino, FreeRTOS, ESP-IDF driver, timer, watchdog, log, or error headers.

The current boundary is source-level and build-enforced. It is designed so scheduler policy and ESP-IDF hardware integration stay outside portable firmware behavior.
