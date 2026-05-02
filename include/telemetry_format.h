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

typedef struct {
    uint32_t timestamp_ms;
    uint32_t heap_free_bytes;
    uint32_t heap_min_free_bytes;
    uint32_t sensor_stack_high_water_words;
    uint32_t telemetry_stack_high_water_words;
    uint32_t button_stack_high_water_words;
    uint32_t fault_stack_high_water_words;
    uint32_t sensor_samples;
    uint32_t telemetry_packets;
    uint32_t button_events;
    uint32_t fault_checks;
    size_t sample_request_queue_depth;
    size_t sensor_sample_queue_depth;
    size_t button_event_queue_depth;
} RuntimeHealthFields;

#ifdef __cplusplus
extern "C" {
#endif

bool format_telemetry_packet(char* buffer, size_t buffer_len, const TelemetryFields* fields);
bool format_fault_event(char* buffer, size_t buffer_len, uint32_t timestamp_ms, FaultCode fault);
bool format_runtime_health_event(char* buffer, size_t buffer_len, const RuntimeHealthFields* fields);
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
