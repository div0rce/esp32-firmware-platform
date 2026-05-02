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

void test_runtime_health_event_format() {
    char buffer[256];
    RuntimeHealthFields fields;
    fields.timestamp_ms = 3000;
    fields.heap_free_bytes = 212000;
    fields.heap_min_free_bytes = 198000;
    fields.sensor_stack_high_water_words = 720;
    fields.telemetry_stack_high_water_words = 740;
    fields.button_stack_high_water_words = 350;
    fields.fault_stack_high_water_words = 310;
    fields.sensor_samples = 30;
    fields.telemetry_packets = 30;
    fields.button_events = 1;
    fields.fault_checks = 60;
    fields.sample_request_queue_depth = 0;
    fields.sensor_sample_queue_depth = 1;
    fields.button_event_queue_depth = 0;

    TEST_ASSERT_TRUE(format_runtime_health_event(buffer, sizeof(buffer), &fields));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=3000,event=RUNTIME_HEALTH,heap_free=212000,heap_min=198000,"
        "stack_sensor=720,stack_telemetry=740,stack_button=350,stack_fault=310,"
        "samples=30,telemetry=30,button_events=1,fault_checks=60,"
        "queue_sample_req=0,queue_sensor=1,queue_button=0",
        buffer
    );
}

void test_self_test_event_format_pass() {
    char buffer[256];

    TEST_ASSERT_TRUE(format_self_test_event(
        buffer,
        sizeof(buffer),
        42,
        "esp32-firmware-platform",
        "dev",
        "seeed_xiao_esp32s3",
        true,
        2050,
        true,
        true,
        true,
        true,
        true,
        true,
        true
    ));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=42,event=SELF_TEST,fw=esp32-firmware-platform,version=dev,target=seeed_xiao_esp32s3,"
        "result=PASS,adc_raw=2050,adc=PASS,button_reg=PASS,button=1,app_state=PASS,watchdog=PASS,queues=PASS,tasks=PASS",
        buffer
    );
}

void test_self_test_event_format_fail() {
    char buffer[256];

    TEST_ASSERT_TRUE(format_self_test_event(
        buffer,
        sizeof(buffer),
        43,
        "esp32-firmware-platform",
        "dev",
        "seeed_xiao_esp32s3",
        false,
        -1,
        false,
        false,
        false,
        true,
        false,
        false,
        false
    ));
    TEST_ASSERT_EQUAL_STRING(
        "timestamp_ms=43,event=SELF_TEST,fw=esp32-firmware-platform,version=dev,target=seeed_xiao_esp32s3,"
        "result=FAIL,adc_raw=-1,adc=FAIL,button_reg=FAIL,button=0,app_state=PASS,watchdog=FAIL,queues=FAIL,tasks=FAIL",
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
    RUN_TEST(test_runtime_health_event_format);
    RUN_TEST(test_self_test_event_format_pass);
    RUN_TEST(test_self_test_event_format_fail);
    RUN_TEST(test_format_rejects_small_buffer);
    return UNITY_END();
}
