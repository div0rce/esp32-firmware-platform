#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "fault_state.h"
#include "sensor_math.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    AdcMovingAverage adc_filter;
    AdcRangeState adc_range_state;
    SystemState state;
    FaultCode fault;
    uint32_t next_sample_ms;
    uint32_t next_telemetry_ms;
    uint32_t last_watchdog_kick_ms;
    uint32_t samples;
    uint32_t telemetry_packets;
    uint32_t fault_checks;
    uint32_t button_events;
    int last_adc_raw;
    int last_voltage_mv;
    bool has_sample;
} AppCoreContext;

void app_core_init(AppCoreContext* context);
void app_core_tick(AppCoreContext* context, uint32_t now_ms);

#ifdef __cplusplus
}
#endif
