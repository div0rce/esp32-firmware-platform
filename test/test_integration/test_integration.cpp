#include <cstdint>

#include <unity.h>

#include "app_config.h"
#include "sensor_math.h"
#include "state_machine.h"
#include "telemetry_format.h"
#include "time_utils.h"

static SystemState apply_sample_result(SystemState state, FaultCode fault) {
    state = state_machine_next(state, APP_EVENT_SAMPLE_STARTED);

    if (fault != FAULT_NONE) {
        return state_machine_next(state, APP_EVENT_FAULT_DETECTED);
    }

    state = state_machine_next(state, APP_EVENT_TRANSMIT_STARTED);
    return state_machine_next(state, APP_EVENT_TRANSMIT_COMPLETE);
}

void test_valid_adc_sample_formats_nominal_telemetry() {
    AdcMovingAverage filter;
    adc_moving_average_init(&filter);

    const int raw = 2048;
    const int filtered_raw = adc_moving_average_update(&filter, raw);
    const FaultCode fault = fault_for_adc_range(adc_range_update(ADC_RANGE_NORMAL, filtered_raw));

    TelemetryFields fields;
    fields.timestamp_ms = 100;
    fields.state = SYSTEM_STATE_TRANSMIT;
    fields.adc_raw = raw;
    fields.voltage_mv = adc_raw_to_mv(filtered_raw);
    fields.fault = fault;

    char buffer[128];

    TEST_ASSERT_EQUAL(FAULT_NONE, fault);
    TEST_ASSERT_TRUE(format_telemetry_packet(buffer, sizeof(buffer), &fields));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=100,state=TRANSMIT,adc_raw=2048,voltage_mv=1650,fault=NONE",
        buffer
    );
}

void test_adc_low_fault_drives_system_fault_state() {
    const FaultCode fault = fault_for_adc_range(
        adc_range_update(ADC_RANGE_NORMAL, ADC_LOW_FAULT_ENTER - 1)
    );

    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, apply_sample_result(SYSTEM_STATE_IDLE, fault));
}

void test_adc_high_fault_drives_system_fault_state() {
    const FaultCode fault = fault_for_adc_range(
        adc_range_update(ADC_RANGE_NORMAL, ADC_HIGH_FAULT_ENTER + 1)
    );

    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, apply_sample_result(SYSTEM_STATE_IDLE, fault));
}

void test_fault_recovery_returns_to_idle() {
    SystemState state = SYSTEM_STATE_FAULT;

    state = state_machine_next(state, APP_EVENT_RECOVERY_REQUESTED);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_RECOVERY, state);

    state = state_machine_next(state, APP_EVENT_RECOVERY_COMPLETE);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, state);
}

void test_stale_sample_timeout_drives_watchdog_fault() {
    const std::uint32_t last_sample_ms = 1000;
    const std::uint32_t now_ms = last_sample_ms + APP_WATCHDOG_TIMEOUT_MS;

    TEST_ASSERT_TRUE(elapsed_ms_u32(now_ms, last_sample_ms, APP_WATCHDOG_TIMEOUT_MS));
    TEST_ASSERT_EQUAL(
        SYSTEM_STATE_FAULT,
        state_machine_next(SYSTEM_STATE_TRANSMIT, APP_EVENT_FAULT_DETECTED)
    );
    TEST_ASSERT_EQUAL_STRING("WATCHDOG_TIMEOUT", fault_to_string(FAULT_WATCHDOG_TIMEOUT));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_valid_adc_sample_formats_nominal_telemetry);
    RUN_TEST(test_adc_low_fault_drives_system_fault_state);
    RUN_TEST(test_adc_high_fault_drives_system_fault_state);
    RUN_TEST(test_fault_recovery_returns_to_idle);
    RUN_TEST(test_stale_sample_timeout_drives_watchdog_fault);
    return UNITY_END();
}
