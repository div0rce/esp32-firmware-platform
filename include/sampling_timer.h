#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

struct SampleRequest {
    std::uint32_t timestamp_ms;
};

bool sampling_timer_start(QueueHandle_t queue);
void sampling_timer_stop();
bool sampling_timer_consume_overflow();
