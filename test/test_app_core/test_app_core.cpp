#include <unity.h>

#include "app_config.h"
#include "app_core.h"
#include "../fakes/fake_platform_hal.h"

#include "../fakes/fake_platform_hal.cpp"
#include "../../components/app_core/src/app_core.cpp"

void test_fake_hal_normal_sampling_emits_telemetry() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    fake_hal_set_adc_raw(2048U);
    app_core_tick(&context, 1U);

    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, context.state);
    TEST_ASSERT_EQUAL(FAULT_NONE, context.fault);
    TEST_ASSERT_EQUAL(1U, context.samples);
    TEST_ASSERT_EQUAL(1U, context.telemetry_packets);
    TEST_ASSERT_EQUAL(1U, fake_hal_get_uart_line_count());
    TEST_ASSERT_NOT_NULL(fake_hal_get_uart_line(0));
    TEST_ASSERT_NOT_NULL(strstr(fake_hal_get_uart_line(0), "state=TRANSMIT"));
    TEST_ASSERT_NOT_NULL(strstr(fake_hal_get_uart_line(0), "fault=NONE"));
    TEST_ASSERT_FALSE(fake_hal_get_gpio_state(PLATFORM_GPIO_STATUS_LED));
}

void test_fake_hal_adc_low_fault_latches() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    fake_hal_set_adc_raw(ADC_LOW_FAULT_ENTER - 1U);
    app_core_tick(&context, 1U);

    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, context.state);
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, context.fault);
    TEST_ASSERT_TRUE(fake_hal_get_gpio_state(PLATFORM_GPIO_STATUS_LED));
    TEST_ASSERT_NOT_NULL(strstr(fake_hal_get_uart_line(0), "state=FAULT"));
    TEST_ASSERT_NOT_NULL(strstr(fake_hal_get_uart_line(0), "fault=ADC_OUT_OF_RANGE"));
}

void test_fake_hal_adc_high_fault_latches() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    fake_hal_set_adc_raw(ADC_HIGH_FAULT_ENTER + 1U);
    app_core_tick(&context, 1U);

    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, context.state);
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, context.fault);
    TEST_ASSERT_TRUE(fake_hal_get_gpio_state(PLATFORM_GPIO_STATUS_LED));
    TEST_ASSERT_NOT_NULL(strstr(fake_hal_get_uart_line(0), "state=FAULT"));
    TEST_ASSERT_NOT_NULL(strstr(fake_hal_get_uart_line(0), "fault=ADC_OUT_OF_RANGE"));
}

void test_fake_hal_button_recovery_clears_recoverable_fault() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    fake_hal_set_adc_raw(ADC_LOW_FAULT_ENTER - 1U);
    app_core_tick(&context, 1U);

    fake_hal_set_adc_raw(2048U);
    fake_hal_set_button_pressed(true);
    app_core_tick(&context, 150U);

    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, context.state);
    TEST_ASSERT_EQUAL(FAULT_NONE, context.fault);
    TEST_ASSERT_EQUAL(1U, context.button_events);
    TEST_ASSERT_FALSE(fake_hal_get_gpio_state(PLATFORM_GPIO_STATUS_LED));
}

void test_fake_hal_uart_failure_is_observable() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    fake_hal_set_uart_write_ok(false);
    app_core_tick(&context, 1U);

    TEST_ASSERT_EQUAL(0U, fake_hal_get_uart_line_count());
    TEST_ASSERT_EQUAL(1U, fake_hal_get_uart_write_failure_count());
    TEST_ASSERT_EQUAL(1U, context.telemetry_packets);
}

void test_fake_hal_watchdog_kick_count_increments() {
    fake_hal_reset();

    TEST_ASSERT_EQUAL(0U, fake_hal_get_watchdog_kick_count());
    platform_watchdog_kick();
    platform_watchdog_kick();
    TEST_ASSERT_EQUAL(2U, fake_hal_get_watchdog_kick_count());
}

void test_fake_hal_trace_pin_toggles_during_tick() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    app_core_tick(&context, 1U);

    TEST_ASSERT_FALSE(fake_hal_get_trace_state(PLATFORM_TRACE_SAMPLE));
    TEST_ASSERT_EQUAL(2U, fake_hal_get_trace_write_count(PLATFORM_TRACE_SAMPLE));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_fake_hal_normal_sampling_emits_telemetry);
    RUN_TEST(test_fake_hal_adc_low_fault_latches);
    RUN_TEST(test_fake_hal_adc_high_fault_latches);
    RUN_TEST(test_fake_hal_button_recovery_clears_recoverable_fault);
    RUN_TEST(test_fake_hal_uart_failure_is_observable);
    RUN_TEST(test_fake_hal_watchdog_kick_count_increments);
    RUN_TEST(test_fake_hal_trace_pin_toggles_during_tick);
    return UNITY_END();
}
