#include "telemetry_format.h"

#include <cstdarg>
#include <cstdio>

static bool write_checked(char* buffer, std::size_t buffer_len, const char* format, ...) {
    if (buffer == nullptr || buffer_len == 0U || format == nullptr) {
        return false;
    }

    va_list args;
    va_start(args, format);
    const int written = std::vsnprintf(buffer, buffer_len, format, args);
    va_end(args);

    return written >= 0 && static_cast<std::size_t>(written) < buffer_len;
}

bool format_telemetry_packet(char* buffer, std::size_t buffer_len, const TelemetryFields& fields) {
    return write_checked(
        buffer,
        buffer_len,
        "timestamp_ms=%lu,state=%s,adc_raw=%d,voltage_mv=%d,fault=%s",
        static_cast<unsigned long>(fields.timestamp_ms),
        state_to_string(fields.state),
        fields.adc_raw,
        fields.voltage_mv,
        fault_to_string(fields.fault)
    );
}

bool format_fault_event(char* buffer, std::size_t buffer_len, std::uint32_t timestamp_ms, FaultCode fault) {
    return write_checked(
        buffer,
        buffer_len,
        "timestamp_ms=%lu,event=FAULT_EVENT,fault=%s",
        static_cast<unsigned long>(timestamp_ms),
        fault_to_string(fault)
    );
}
