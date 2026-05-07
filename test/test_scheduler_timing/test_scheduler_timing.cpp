#include <limits>

#include <unity.h>

#include "app_config.h"
#include "app_core.h"
#include "time_utils.h"
#include "../fakes/fake_platform_hal.h"

#include "../fakes/fake_platform_hal.cpp"
#include "../../components/app_core/src/app_core.cpp"

void test_scheduler_does_not_sample_before_period() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    app_core_tick(&context, 1U);
    app_core_tick(&context, SAMPLE_PERIOD_MS);

    TEST_ASSERT_EQUAL(1U, context.samples);
    TEST_ASSERT_EQUAL(1U + SAMPLE_PERIOD_MS, context.next_sample_ms);
    TEST_ASSERT_EQUAL(1U, fake_hal_get_uart_line_count());
}

void test_scheduler_samples_at_period_boundary() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    app_core_tick(&context, 1U);
    app_core_tick(&context, 1U + SAMPLE_PERIOD_MS);

    TEST_ASSERT_EQUAL(2U, context.samples);
    TEST_ASSERT_EQUAL(2U, fake_hal_get_uart_line_count());
    TEST_ASSERT_EQUAL(1U + (2U * SAMPLE_PERIOD_MS), context.next_sample_ms);
}

void test_scheduler_samples_once_per_elapsed_period() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    app_core_tick(&context, 1U);
    app_core_tick(&context, 1U + (5U * SAMPLE_PERIOD_MS));

    TEST_ASSERT_EQUAL(2U, context.samples);
    TEST_ASSERT_EQUAL(1U + (6U * SAMPLE_PERIOD_MS), context.next_sample_ms);

    app_core_tick(&context, 2U + (5U * SAMPLE_PERIOD_MS));
    TEST_ASSERT_EQUAL(2U, context.samples);
}

void test_scheduler_handles_u32_rollover_at_sample_boundary() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    const uint32_t scheduled = std::numeric_limits<uint32_t>::max() - 5U;
    context.next_sample_ms = scheduled;

    app_core_tick(&context, scheduled - 1U);
    TEST_ASSERT_EQUAL(0U, context.samples);

    app_core_tick(&context, scheduled);
    TEST_ASSERT_EQUAL(1U, context.samples);
    TEST_ASSERT_EQUAL(scheduled + SAMPLE_PERIOD_MS, context.next_sample_ms);
}

void test_scheduler_button_recovery_does_not_wait_for_sample_period() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    fake_hal_set_adc_raw(ADC_LOW_FAULT_ENTER - 1U);
    app_core_tick(&context, 1U);

    fake_hal_set_adc_raw(2048U);
    fake_hal_set_button_pressed(true);
    app_core_tick(&context, 50U);

    TEST_ASSERT_EQUAL(1U, context.samples);
    TEST_ASSERT_EQUAL(1U, context.button_events);
    TEST_ASSERT_EQUAL(FAULT_NONE, context.fault);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, context.state);
}

void test_scheduler_watchdog_service_bookkeeping_updates_each_tick() {
    fake_hal_reset();
    AppCoreContext context;
    app_core_init(&context);

    app_core_tick(&context, 1U);
    app_core_tick(&context, 50U);

    TEST_ASSERT_EQUAL(2U, context.fault_checks);
    TEST_ASSERT_EQUAL(50U, context.last_watchdog_kick_ms);
    TEST_ASSERT_FALSE(elapsed_ms_u32(50U, context.last_watchdog_kick_ms, APP_WATCHDOG_TIMEOUT_MS));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_scheduler_does_not_sample_before_period);
    RUN_TEST(test_scheduler_samples_at_period_boundary);
    RUN_TEST(test_scheduler_samples_once_per_elapsed_period);
    RUN_TEST(test_scheduler_handles_u32_rollover_at_sample_boundary);
    RUN_TEST(test_scheduler_button_recovery_does_not_wait_for_sample_period);
    RUN_TEST(test_scheduler_watchdog_service_bookkeeping_updates_each_tick);
    return UNITY_END();
}
