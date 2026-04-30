#include <unity.h>

#include "telemetry_format.h"

void test_telemetry_packet_format() {
    char buffer[128];
    TelemetryFields fields;
    fields.timestamp_ms = 12500;
    fields.state = SystemState::SAMPLE;
    fields.adc_raw = 1842;
    fields.voltage_mv = 1485;
    fields.fault = FaultCode::NONE;

    TEST_ASSERT_TRUE(format_telemetry_packet(buffer, sizeof(buffer), fields));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=12500,state=SAMPLE,adc_raw=1842,voltage_mv=1485,fault=NONE",
        buffer
    );
}

void test_fault_event_format() {
    char buffer[96];

    TEST_ASSERT_TRUE(format_fault_event(buffer, sizeof(buffer), 12700, FaultCode::ADC_OUT_OF_RANGE));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=12700,event=FAULT_EVENT,fault=ADC_OUT_OF_RANGE",
        buffer
    );
}

void test_format_rejects_small_buffer() {
    char buffer[8];
    TelemetryFields fields;
    fields.timestamp_ms = 1;
    fields.state = SystemState::IDLE;
    fields.adc_raw = 1;
    fields.voltage_mv = 1;
    fields.fault = FaultCode::NONE;

    TEST_ASSERT_FALSE(format_telemetry_packet(buffer, sizeof(buffer), fields));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_telemetry_packet_format);
    RUN_TEST(test_fault_event_format);
    RUN_TEST(test_format_rejects_small_buffer);
    return UNITY_END();
}
