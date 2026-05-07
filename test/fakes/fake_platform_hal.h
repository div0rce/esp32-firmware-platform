#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "platform_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void fake_hal_reset(void);
void fake_hal_set_now_ms(uint32_t now_ms);
void fake_hal_advance_ms(uint32_t delta_ms);
void fake_hal_set_adc_raw(uint16_t raw);
void fake_hal_set_adc_read_ok(bool ok);
void fake_hal_set_button_pressed(bool pressed);
void fake_hal_set_uart_write_ok(bool ok);
void fake_hal_set_reset_reason(platform_reset_reason_t reason);

const char* fake_hal_get_uart_line(size_t index);
size_t fake_hal_get_uart_line_count(void);
uint32_t fake_hal_get_uart_write_failure_count(void);
uint32_t fake_hal_get_watchdog_kick_count(void);
bool fake_hal_get_gpio_state(platform_gpio_id_t pin);
uint32_t fake_hal_get_gpio_write_count(platform_gpio_id_t pin);
bool fake_hal_get_trace_state(platform_trace_id_t trace);
uint32_t fake_hal_get_trace_write_count(platform_trace_id_t trace);

#ifdef __cplusplus
}
#endif
