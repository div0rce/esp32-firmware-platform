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
