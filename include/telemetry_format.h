#pragma once

#include <cstddef>
#include <cstdint>

#include "fault_state.h"

struct TelemetryFields {
    std::uint32_t timestamp_ms;
    SystemState state;
    int adc_raw;
    int voltage_mv;
    FaultCode fault;
};

bool format_telemetry_packet(char* buffer, std::size_t buffer_len, const TelemetryFields& fields);
bool format_fault_event(char* buffer, std::size_t buffer_len, std::uint32_t timestamp_ms, FaultCode fault);
