#include <unity.h>

#include "telemetry_format.h"

void test_telemetry_packet_format() {
    char buffer[128];
    TelemetryFields fields;
    fields.timestamp_ms = 12500;
    fields.state = SYSTEM_STATE_SAMPLE;
    fields.adc_raw = 1842;
    fields.voltage_mv = 1485;
    fields.fault = FAULT_NONE;

    TEST_ASSERT_TRUE(format_telemetry_packet(buffer, sizeof(buffer), &fields));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=12500,state=SAMPLE,adc_raw=1842,voltage_mv=1485,fault=NONE",
        buffer
    );
}

void test_fault_event_format() {
    char buffer[96];

    TEST_ASSERT_TRUE(format_fault_event(buffer, sizeof(buffer), 12700, FAULT_ADC_OUT_OF_RANGE));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=12700,event=FAULT_EVENT,fault=ADC_OUT_OF_RANGE",
        buffer
    );
}

void test_self_test_event_format_pass() {
    char buffer[128];

    TEST_ASSERT_TRUE(format_self_test_event(buffer, sizeof(buffer), 42, true, 2050, true));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=42,event=SELF_TEST,result=PASS,adc_raw=2050,adc_raw_in_range=PASS",
        buffer
    );
}

void test_self_test_event_format_fail() {
    char buffer[128];

    TEST_ASSERT_TRUE(format_self_test_event(buffer, sizeof(buffer), 43, false, -1, false));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=43,event=SELF_TEST,result=FAIL,adc_raw=-1,adc_raw_in_range=FAIL",
        buffer
    );
}

void test_format_rejects_small_buffer() {
    char buffer[8];
    TelemetryFields fields;
    fields.timestamp_ms = 1;
    fields.state = SYSTEM_STATE_IDLE;
    fields.adc_raw = 1;
    fields.voltage_mv = 1;
    fields.fault = FAULT_NONE;

    TEST_ASSERT_FALSE(format_telemetry_packet(buffer, sizeof(buffer), &fields));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_telemetry_packet_format);
    RUN_TEST(test_fault_event_format);
    RUN_TEST(test_self_test_event_format_pass);
    RUN_TEST(test_self_test_event_format_fail);
    RUN_TEST(test_format_rejects_small_buffer);
    return UNITY_END();
}
