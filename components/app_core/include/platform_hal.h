#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PLATFORM_GPIO_STATUS_LED = 0,
    PLATFORM_GPIO_BUTTON
} platform_gpio_id_t;

typedef enum {
    PLATFORM_TRACE_SAMPLE = 0,
    PLATFORM_TRACE_TELEMETRY,
    PLATFORM_TRACE_FAULT,
    PLATFORM_TRACE_LOOP
} platform_trace_id_t;

typedef enum {
    PLATFORM_RESET_REASON_UNKNOWN = 0,
    PLATFORM_RESET_REASON_POWER_ON,
    PLATFORM_RESET_REASON_SOFTWARE,
    PLATFORM_RESET_REASON_WATCHDOG,
    PLATFORM_RESET_REASON_EXTERNAL,
    PLATFORM_RESET_REASON_DEEP_SLEEP
} platform_reset_reason_t;

bool platform_init(void);
uint32_t platform_time_now_ms(void);
void platform_time_sleep_ms(uint32_t delay_ms);
bool platform_adc_read_raw(uint16_t* raw_out);
bool platform_uart_write_line(const char* line);
void platform_gpio_write(platform_gpio_id_t pin, bool value);
bool platform_gpio_read(platform_gpio_id_t pin);
bool platform_button_read(void);
void platform_trace_set(platform_trace_id_t trace, bool value);
void platform_watchdog_kick(void);
platform_reset_reason_t platform_reset_reason_get(void);

#ifdef __cplusplus
}
#endif
