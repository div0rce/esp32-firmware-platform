#include <cstring>

#include <unity.h>

#include "app_config.h"
#include "app_core.h"
#include "platform_hal.h"

namespace {

bool adc_read_ok = true;
uint16_t adc_raw = 2048;
bool button_pressed = false;
bool status_led = false;
char last_uart_line[TELEMETRY_BUFFER_LENGTH];

void reset_platform_fixture() {
    adc_read_ok = true;
    adc_raw = 2048;
    button_pressed = false;
    status_led = false;
    last_uart_line[0] = '\0';
}

}

extern "C" bool platform_init(void) {
    return true;
}

extern "C" uint32_t platform_time_now_ms(void) {
    return 0;
}

extern "C" void platform_time_sleep_ms(uint32_t delay_ms) {
    (void)delay_ms;
}

extern "C" bool platform_adc_read_raw(uint16_t* raw_out) {
    if (!adc_read_ok || raw_out == nullptr) {
        return false;
    }

    *raw_out = adc_raw;
    return true;
}

extern "C" bool platform_uart_write_line(const char* line) {
    if (line == nullptr) {
        return false;
    }

    std::strncpy(last_uart_line, line, sizeof(last_uart_line) - 1U);
    last_uart_line[sizeof(last_uart_line) - 1U] = '\0';
    return true;
}

extern "C" void platform_gpio_write(platform_gpio_id_t pin, bool value) {
    if (pin == PLATFORM_GPIO_STATUS_LED) {
        status_led = value;
    }
}

extern "C" bool platform_gpio_read(platform_gpio_id_t pin) {
    return pin == PLATFORM_GPIO_STATUS_LED ? status_led : false;
}

extern "C" bool platform_button_read(void) {
    return button_pressed;
}

extern "C" void platform_trace_set(platform_trace_id_t trace, bool value) {
    (void)trace;
    (void)value;
}

extern "C" void platform_watchdog_kick(void) {}

extern "C" platform_reset_reason_t platform_reset_reason_get(void) {
    return PLATFORM_RESET_REASON_POWER_ON;
}

#include "../../components/app_core/src/app_core.cpp"

void test_cooperative_adc_fault_latches_until_button_recovery() {
    reset_platform_fixture();

    AppCoreContext context;
    app_core_init(&context);

    adc_raw = ADC_LOW_FAULT_ENTER - 1U;
    app_core_tick(&context, 1U);

    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, context.state);
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, context.fault);
    TEST_ASSERT_TRUE(status_led);

    adc_raw = 2048U;
    app_core_tick(&context, 101U);

    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, context.state);
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, context.fault);
    TEST_ASSERT_TRUE(status_led);
    TEST_ASSERT_NOT_NULL(std::strstr(last_uart_line, "state=FAULT"));
    TEST_ASSERT_NOT_NULL(std::strstr(last_uart_line, "fault=ADC_OUT_OF_RANGE"));

    button_pressed = true;
    app_core_tick(&context, 150U);

    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, context.state);
    TEST_ASSERT_EQUAL(FAULT_NONE, context.fault);
    TEST_ASSERT_EQUAL(1U, context.button_events);
    TEST_ASSERT_FALSE(status_led);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_cooperative_adc_fault_latches_until_button_recovery);
    return UNITY_END();
}
