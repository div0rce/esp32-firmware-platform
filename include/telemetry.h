#pragma once

#include <cstdint>

#include "fault_state.h"
#include "manufacturing_self_test.h"
#include "sensor.h"
#include "telemetry_format.h"

void telemetry_init();
void telemetry_print_sample(SystemState state, const SensorSample& sample);
void telemetry_print_fault_event(std::uint32_t timestamp_ms, FaultCode fault);
void telemetry_print_runtime_health(const RuntimeHealthFields& fields);
void telemetry_print_self_test_result(std::uint32_t timestamp_ms, const ManufacturingSelfTestResult& result);
