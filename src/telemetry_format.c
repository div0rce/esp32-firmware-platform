#include "telemetry_format.h"

#include <stdarg.h>
#include <stdio.h>

static bool write_checked(char* buffer, size_t buffer_len, const char* format, ...) {
    if (buffer == NULL || buffer_len == 0U || format == NULL) {
        return false;
    }

    va_list args;
    va_start(args, format);
    const int written = vsnprintf(buffer, buffer_len, format, args);
    va_end(args);

    return written >= 0 && (size_t)written < buffer_len;
}

bool format_telemetry_packet(char* buffer, size_t buffer_len, const TelemetryFields* fields) {
    if (fields == NULL) {
        return false;
    }

    return write_checked(
        buffer,
        buffer_len,
        "timestamp_ms=%lu,state=%s,adc_raw=%d,voltage_mv=%d,fault=%s",
        (unsigned long)fields->timestamp_ms,
        state_to_string(fields->state),
        fields->adc_raw,
        fields->voltage_mv,
        fault_to_string(fields->fault)
    );
}

bool format_fault_event(char* buffer, size_t buffer_len, uint32_t timestamp_ms, FaultCode fault) {
    return write_checked(
        buffer,
        buffer_len,
        "timestamp_ms=%lu,event=FAULT_EVENT,fault=%s",
        (unsigned long)timestamp_ms,
        fault_to_string(fault)
    );
}

bool format_runtime_health_event(char* buffer, size_t buffer_len, const RuntimeHealthFields* fields) {
    if (fields == NULL) {
        return false;
    }

    return write_checked(
        buffer,
        buffer_len,
        "timestamp_ms=%lu,event=RUNTIME_HEALTH,heap_free=%lu,heap_min=%lu,"
        "stack_sensor=%lu,stack_telemetry=%lu,stack_button=%lu,stack_fault=%lu,"
        "samples=%lu,telemetry=%lu,button_events=%lu,fault_checks=%lu,"
        "queue_sample_req=%lu,queue_sensor=%lu,queue_button=%lu",
        (unsigned long)fields->timestamp_ms,
        (unsigned long)fields->heap_free_bytes,
        (unsigned long)fields->heap_min_free_bytes,
        (unsigned long)fields->sensor_stack_high_water_words,
        (unsigned long)fields->telemetry_stack_high_water_words,
        (unsigned long)fields->button_stack_high_water_words,
        (unsigned long)fields->fault_stack_high_water_words,
        (unsigned long)fields->sensor_samples,
        (unsigned long)fields->telemetry_packets,
        (unsigned long)fields->button_events,
        (unsigned long)fields->fault_checks,
        (unsigned long)fields->sample_request_queue_depth,
        (unsigned long)fields->sensor_sample_queue_depth,
        (unsigned long)fields->button_event_queue_depth
    );
}

bool format_self_test_event(
    char* buffer,
    size_t buffer_len,
    uint32_t timestamp_ms,
    bool passed,
    int adc_raw,
    bool adc_raw_in_range,
    bool button_gpio_read_ok,
    bool button_level
) {
    return write_checked(
        buffer,
        buffer_len,
        "timestamp_ms=%lu,event=SELF_TEST,result=%s,adc_raw=%d,adc=%s,button_reg=%s,button=%u",
        (unsigned long)timestamp_ms,
        passed ? "PASS" : "FAIL",
        adc_raw,
        adc_raw_in_range ? "PASS" : "FAIL",
        button_gpio_read_ok ? "PASS" : "FAIL",
        button_level ? 1U : 0U
    );
}
