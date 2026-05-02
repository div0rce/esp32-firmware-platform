#include "runtime_health.h"

#include <esp_system.h>

static portMUX_TYPE counters_mux = portMUX_INITIALIZER_UNLOCKED;
static std::uint32_t sensor_samples = 0;
static std::uint32_t telemetry_packets = 0;
static std::uint32_t button_events = 0;
static std::uint32_t fault_checks = 0;

static std::uint32_t stack_high_water_words(TaskHandle_t task) {
    if (task == nullptr) {
        return 0;
    }

    return static_cast<std::uint32_t>(uxTaskGetStackHighWaterMark(task));
}

static size_t queue_depth(QueueHandle_t queue) {
    if (queue == nullptr) {
        return 0;
    }

    return static_cast<size_t>(uxQueueMessagesWaiting(queue));
}

static void increment_counter(std::uint32_t* counter) {
    portENTER_CRITICAL(&counters_mux);
    *counter += 1U;
    portEXIT_CRITICAL(&counters_mux);
}

void runtime_health_record_sensor_sample() {
    increment_counter(&sensor_samples);
}

void runtime_health_record_telemetry_packet() {
    increment_counter(&telemetry_packets);
}

void runtime_health_record_button_event() {
    increment_counter(&button_events);
}

void runtime_health_record_fault_check() {
    increment_counter(&fault_checks);
}

RuntimeHealthFields runtime_health_capture(const RuntimeHealthInputs& inputs) {
    RuntimeHealthFields fields;
    fields.timestamp_ms = inputs.timestamp_ms;
    fields.heap_free_bytes = static_cast<std::uint32_t>(esp_get_free_heap_size());
    fields.heap_min_free_bytes = static_cast<std::uint32_t>(esp_get_minimum_free_heap_size());
    fields.sensor_stack_high_water_words = stack_high_water_words(inputs.sensor_task);
    fields.telemetry_stack_high_water_words = stack_high_water_words(inputs.telemetry_task);
    fields.button_stack_high_water_words = stack_high_water_words(inputs.button_task);
    fields.fault_stack_high_water_words = stack_high_water_words(inputs.fault_task);
    fields.sample_request_queue_depth = queue_depth(inputs.sample_request_queue);
    fields.sensor_sample_queue_depth = queue_depth(inputs.sensor_sample_queue);
    fields.button_event_queue_depth = queue_depth(inputs.button_event_queue);

    portENTER_CRITICAL(&counters_mux);
    fields.sensor_samples = sensor_samples;
    fields.telemetry_packets = telemetry_packets;
    fields.button_events = button_events;
    fields.fault_checks = fault_checks;
    portEXIT_CRITICAL(&counters_mux);

    return fields;
}
