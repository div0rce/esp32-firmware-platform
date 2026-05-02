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
);

#ifdef __cplusplus
}
#endif
