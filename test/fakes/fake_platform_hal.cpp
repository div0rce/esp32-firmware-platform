#include "fake_platform_hal.h"

#include <string.h>

#include "app_config.h"

namespace {

constexpr size_t kMaxUartLines = 16;
constexpr size_t kGpioCount = 2;
constexpr size_t kTraceCount = 4;

struct FakeHalState {
    uint32_t now_ms;
    uint16_t adc_raw;
    bool adc_read_ok;
    bool button_pressed;
    bool uart_write_ok;
    platform_reset_reason_t reset_reason;
    char uart_lines[kMaxUartLines][TELEMETRY_BUFFER_LENGTH];
    size_t uart_line_count;
    uint32_t uart_write_failure_count;
    uint32_t watchdog_kick_count;
    bool gpio_state[kGpioCount];
    uint32_t gpio_write_count[kGpioCount];
    bool trace_state[kTraceCount];
    uint32_t trace_write_count[kTraceCount];
};

FakeHalState state;

size_t gpio_index(platform_gpio_id_t pin) {
    return static_cast<size_t>(pin);
}

size_t trace_index(platform_trace_id_t trace) {
    return static_cast<size_t>(trace);
}

}

extern "C" void fake_hal_reset(void) {
    memset(&state, 0, sizeof(state));
    state.adc_raw = 2048U;
    state.adc_read_ok = true;
    state.uart_write_ok = true;
    state.reset_reason = PLATFORM_RESET_REASON_POWER_ON;
}

extern "C" void fake_hal_set_now_ms(uint32_t now_ms) {
    state.now_ms = now_ms;
}

extern "C" void fake_hal_advance_ms(uint32_t delta_ms) {
    state.now_ms += delta_ms;
}

extern "C" void fake_hal_set_adc_raw(uint16_t raw) {
    state.adc_raw = raw;
}

extern "C" void fake_hal_set_adc_read_ok(bool ok) {
    state.adc_read_ok = ok;
}

extern "C" void fake_hal_set_button_pressed(bool pressed) {
    state.button_pressed = pressed;
}

extern "C" void fake_hal_set_uart_write_ok(bool ok) {
    state.uart_write_ok = ok;
}

extern "C" void fake_hal_set_reset_reason(platform_reset_reason_t reason) {
    state.reset_reason = reason;
}

extern "C" const char* fake_hal_get_uart_line(size_t index) {
    if (index >= state.uart_line_count) {
        return nullptr;
    }

    return state.uart_lines[index];
}

extern "C" size_t fake_hal_get_uart_line_count(void) {
    return state.uart_line_count;
}

extern "C" uint32_t fake_hal_get_uart_write_failure_count(void) {
    return state.uart_write_failure_count;
}

extern "C" uint32_t fake_hal_get_watchdog_kick_count(void) {
    return state.watchdog_kick_count;
}

extern "C" bool fake_hal_get_gpio_state(platform_gpio_id_t pin) {
    const size_t index = gpio_index(pin);
    return index < kGpioCount && state.gpio_state[index];
}

extern "C" uint32_t fake_hal_get_gpio_write_count(platform_gpio_id_t pin) {
    const size_t index = gpio_index(pin);
    return index < kGpioCount ? state.gpio_write_count[index] : 0U;
}

extern "C" bool fake_hal_get_trace_state(platform_trace_id_t trace) {
    const size_t index = trace_index(trace);
    return index < kTraceCount && state.trace_state[index];
}

extern "C" uint32_t fake_hal_get_trace_write_count(platform_trace_id_t trace) {
    const size_t index = trace_index(trace);
    return index < kTraceCount ? state.trace_write_count[index] : 0U;
}

extern "C" bool platform_init(void) {
    return true;
}

extern "C" uint32_t platform_time_now_ms(void) {
    return state.now_ms;
}

extern "C" void platform_time_sleep_ms(uint32_t delay_ms) {
    fake_hal_advance_ms(delay_ms);
}

extern "C" bool platform_adc_read_raw(uint16_t* raw_out) {
    if (!state.adc_read_ok || raw_out == nullptr) {
        return false;
    }

    *raw_out = state.adc_raw;
    return true;
}

extern "C" bool platform_uart_write_line(const char* line) {
    if (!state.uart_write_ok || line == nullptr) {
        state.uart_write_failure_count += 1U;
        return false;
    }

    if (state.uart_line_count < kMaxUartLines) {
        strncpy(state.uart_lines[state.uart_line_count], line, TELEMETRY_BUFFER_LENGTH - 1U);
        state.uart_lines[state.uart_line_count][TELEMETRY_BUFFER_LENGTH - 1U] = '\0';
        state.uart_line_count += 1U;
    }

    return true;
}

extern "C" void platform_gpio_write(platform_gpio_id_t pin, bool value) {
    const size_t index = gpio_index(pin);
    if (index >= kGpioCount) {
        return;
    }

    state.gpio_state[index] = value;
    state.gpio_write_count[index] += 1U;
}

extern "C" bool platform_gpio_read(platform_gpio_id_t pin) {
    return fake_hal_get_gpio_state(pin);
}

extern "C" bool platform_button_read(void) {
    return state.button_pressed;
}

extern "C" void platform_trace_set(platform_trace_id_t trace, bool value) {
    const size_t index = trace_index(trace);
    if (index >= kTraceCount) {
        return;
    }

    state.trace_state[index] = value;
    state.trace_write_count[index] += 1U;
}

extern "C" void platform_watchdog_kick(void) {
    state.watchdog_kick_count += 1U;
}

extern "C" platform_reset_reason_t platform_reset_reason_get(void) {
    return state.reset_reason;
}
