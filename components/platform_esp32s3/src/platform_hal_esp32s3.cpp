#include "platform_hal.h"

#include <cstdint>

#include "app_config.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "firmware_hal.h"

namespace {

std::uint8_t gpio_pin_for_id(platform_gpio_id_t pin) {
    switch (pin) {
        case PLATFORM_GPIO_BUTTON:
            return BUTTON_PIN;
        case PLATFORM_GPIO_STATUS_LED:
        default:
            return STATUS_LED_PIN;
    }
}

std::uint8_t trace_pin_for_id(platform_trace_id_t trace) {
    switch (trace) {
        case PLATFORM_TRACE_TELEMETRY:
            return TRACE_TELEMETRY_PIN;
        case PLATFORM_TRACE_FAULT:
            return TRACE_FAULT_PIN;
        case PLATFORM_TRACE_LOOP:
        case PLATFORM_TRACE_SAMPLE:
        default:
            return TRACE_SAMPLE_PIN;
    }
}

platform_reset_reason_t map_reset_reason(esp_reset_reason_t reason) {
    switch (reason) {
        case ESP_RST_POWERON:
            return PLATFORM_RESET_REASON_POWER_ON;
        case ESP_RST_SW:
            return PLATFORM_RESET_REASON_SOFTWARE;
        case ESP_RST_PANIC:
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:
            return PLATFORM_RESET_REASON_WATCHDOG;
        case ESP_RST_EXT:
            return PLATFORM_RESET_REASON_EXTERNAL;
        case ESP_RST_DEEPSLEEP:
            return PLATFORM_RESET_REASON_DEEP_SLEEP;
        default:
            return PLATFORM_RESET_REASON_UNKNOWN;
    }
}

}

bool platform_init(void) {
    firmware_hal::uart_begin(SERIAL_BAUD_RATE);
    firmware_hal::adc_configure_resolution(ADC_RESOLUTION_BITS);
    firmware_hal::gpio_configure_output(STATUS_LED_PIN);
    firmware_hal::gpio_configure_input_pullup(BUTTON_PIN);
    firmware_hal::gpio_configure_output(TRACE_SAMPLE_PIN);
    firmware_hal::gpio_configure_output(TRACE_TELEMETRY_PIN);
    firmware_hal::gpio_configure_output(TRACE_FAULT_PIN);

    firmware_hal::gpio_write_level(STATUS_LED_PIN, false);
    firmware_hal::gpio_write_level(TRACE_SAMPLE_PIN, false);
    firmware_hal::gpio_write_level(TRACE_TELEMETRY_PIN, false);
    firmware_hal::gpio_write_level(TRACE_FAULT_PIN, false);

    esp_task_wdt_config_t config = {};
    config.timeout_ms = TASK_WATCHDOG_TIMEOUT_SECONDS * 1000U;
    config.idle_core_mask = 0;
    config.trigger_panic = true;

    const esp_err_t init_result = esp_task_wdt_init(&config);
    if (init_result != ESP_OK && init_result != ESP_ERR_INVALID_STATE) {
        return false;
    }

    const esp_err_t add_result = esp_task_wdt_add(nullptr);
    return add_result == ESP_OK || add_result == ESP_ERR_INVALID_STATE;
}

uint32_t platform_time_now_ms(void) {
    return firmware_hal::monotonic_millis();
}

void platform_time_sleep_ms(uint32_t delay_ms) {
    firmware_hal::sleep_ms(delay_ms);
}

bool platform_adc_read_raw(uint16_t* raw_out) {
    if (raw_out == nullptr) {
        return false;
    }

    const int raw = firmware_hal::adc_read_raw(ADC_PIN);
    if (raw < ADC_RAW_MIN || raw > ADC_RAW_MAX) {
        return false;
    }

    *raw_out = static_cast<uint16_t>(raw);
    return true;
}

bool platform_uart_write_line(const char* line) {
    if (line == nullptr) {
        return false;
    }

    firmware_hal::uart_println(line);
    return true;
}

void platform_gpio_write(platform_gpio_id_t pin, bool value) {
    firmware_hal::gpio_write_level(gpio_pin_for_id(pin), value);
}

bool platform_gpio_read(platform_gpio_id_t pin) {
    bool level = false;
    (void)firmware_hal::gpio_read_input_register(gpio_pin_for_id(pin), &level);
    return level;
}

bool platform_button_read(void) {
    return !platform_gpio_read(PLATFORM_GPIO_BUTTON);
}

void platform_trace_set(platform_trace_id_t trace, bool value) {
    firmware_hal::gpio_write_level(trace_pin_for_id(trace), value);
}

void platform_watchdog_kick(void) {
    (void)esp_task_wdt_reset();
}

platform_reset_reason_t platform_reset_reason_get(void) {
    return map_reset_reason(esp_reset_reason());
}
