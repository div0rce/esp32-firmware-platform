#pragma once

#include <cstdint>

#include "fault_state.h"
#include "sampling_timer.h"

struct SensorSample {
    std::uint32_t timestamp_ms;
    int adc_raw;
    int voltage_mv;
    FaultCode fault;
};

void sensor_init();
SensorSample read_sensor_sample(const SampleRequest& request);
bool sample_is_valid(const SensorSample& sample);
