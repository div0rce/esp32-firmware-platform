#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "fault_state.h"

struct StressModeState {
    std::uint32_t last_recovery_inject_ms;
    std::uint32_t last_overflow_inject_ms;
};

void stress_mode_init(StressModeState* state);
FaultCode stress_mode_forced_sample_fault(std::uint32_t timestamp_ms);
void stress_mode_delay_telemetry();
void stress_mode_service(StressModeState* state, std::uint32_t now_ms, QueueHandle_t button_event_queue);
