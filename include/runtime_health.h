#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "telemetry_format.h"

struct RuntimeHealthInputs {
    std::uint32_t timestamp_ms;
    TaskHandle_t sensor_task;
    TaskHandle_t telemetry_task;
    TaskHandle_t button_task;
    TaskHandle_t fault_task;
    QueueHandle_t sample_request_queue;
    QueueHandle_t sensor_sample_queue;
    QueueHandle_t button_event_queue;
};

void runtime_health_record_sensor_sample();
void runtime_health_record_telemetry_packet();
void runtime_health_record_button_event();
void runtime_health_record_fault_check();
RuntimeHealthFields runtime_health_capture(const RuntimeHealthInputs& inputs);
