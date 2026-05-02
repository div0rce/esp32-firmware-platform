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

bool format_self_test_event(
    char* buffer,
    size_t buffer_len,
    uint32_t timestamp_ms,
    const char* firmware_name,
    const char* firmware_version,
    const char* firmware_target,
    bool passed,
    int adc_raw,
    bool adc_raw_in_range,
    bool button_gpio_read_ok,
    bool button_level,
    bool app_state_init_ok,
    bool watchdog_init_ok,
    bool queue_create_ok,
    bool task_create_ok
) {
    if (firmware_name == NULL || firmware_version == NULL || firmware_target == NULL) {
        return false;
    }

    return write_checked(
        buffer,
        buffer_len,
        "timestamp_ms=%lu,event=SELF_TEST,fw=%s,version=%s,target=%s,result=%s,"
        "adc_raw=%d,adc=%s,button_reg=%s,button=%u,app_state=%s,watchdog=%s,queues=%s,tasks=%s",
        (unsigned long)timestamp_ms,
        firmware_name,
        firmware_version,
        firmware_target,
        passed ? "PASS" : "FAIL",
        adc_raw,
        adc_raw_in_range ? "PASS" : "FAIL",
        button_gpio_read_ok ? "PASS" : "FAIL",
        button_level ? 1U : 0U,
        app_state_init_ok ? "PASS" : "FAIL",
        watchdog_init_ok ? "PASS" : "FAIL",
        queue_create_ok ? "PASS" : "FAIL",
        task_create_ok ? "PASS" : "FAIL"
    );
}
