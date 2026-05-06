#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "sample_request.h"

bool sampling_timer_start(QueueHandle_t queue);
void sampling_timer_stop();
bool sampling_timer_consume_overflow();

#if defined(ENABLE_STRESS_MODE)
void sampling_timer_stress_mark_overflow();
#endif
