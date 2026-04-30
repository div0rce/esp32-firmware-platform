#pragma once

#include <cstdint>

#include "fault_state.h"
#include "sensor.h"

void telemetry_init();
void telemetry_print_sample(SystemState state, const SensorSample& sample);
void telemetry_print_fault_event(std::uint32_t timestamp_ms, FaultCode fault);
