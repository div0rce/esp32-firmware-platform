#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "fault_state.h"

typedef struct {
    uint32_t timestamp_ms;
    SystemState state;
    int adc_raw;
    int voltage_mv;
    FaultCode fault;
} TelemetryFields;

#ifdef __cplusplus
extern "C" {
#endif

bool format_telemetry_packet(char* buffer, size_t buffer_len, const TelemetryFields* fields);
bool format_fault_event(char* buffer, size_t buffer_len, uint32_t timestamp_ms, FaultCode fault);
bool format_self_test_event(
    char* buffer,
    size_t buffer_len,
    uint32_t timestamp_ms,
    bool passed,
    int adc_raw,
    bool adc_raw_in_range
);

#ifdef __cplusplus
}
#endif
