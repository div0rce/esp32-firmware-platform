#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

struct ButtonEvent {
    std::uint32_t timestamp_ms;
};

void button_init(QueueHandle_t queue);
bool button_receive_event(ButtonEvent* event, TickType_t timeout_ticks);
bool button_consume_overflow();

#if defined(ENABLE_STRESS_MODE)
void button_stress_mark_overflow();
#endif
