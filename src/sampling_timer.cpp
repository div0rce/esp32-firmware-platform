#include "sampling_timer.h"

#include <esp_timer.h>

#include "app_config.h"

static esp_timer_handle_t sampling_timer = nullptr;
static portMUX_TYPE overflow_mux = portMUX_INITIALIZER_UNLOCKED;
static bool sample_request_overflow = false;

static void sampling_timer_set_overflow() {
    portENTER_CRITICAL(&overflow_mux);
    sample_request_overflow = true;
    portEXIT_CRITICAL(&overflow_mux);
}

static void sampling_timer_callback(void* arg) {
    QueueHandle_t queue = static_cast<QueueHandle_t>(arg);

    if (queue == nullptr) {
        sampling_timer_set_overflow();
        return;
    }

    SampleRequest request;
    request.timestamp_ms = static_cast<std::uint32_t>(esp_timer_get_time() / 1000LL);

    if (xQueueSend(queue, &request, 0) != pdPASS) {
        sampling_timer_set_overflow();
    }
}

bool sampling_timer_start(QueueHandle_t queue) {
    if (queue == nullptr) {
        return false;
    }

    if (sampling_timer == nullptr) {
        esp_timer_create_args_t timer_args = {};
        timer_args.callback = sampling_timer_callback;
        timer_args.arg = queue;
        timer_args.dispatch_method = ESP_TIMER_TASK;
        timer_args.name = "sampling_timer";

        if (esp_timer_create(&timer_args, &sampling_timer) != ESP_OK) {
            return false;
        }
    }

    return esp_timer_start_periodic(
        sampling_timer,
        static_cast<std::uint64_t>(SAMPLE_PERIOD_MS) * 1000ULL
    ) == ESP_OK;
}

void sampling_timer_stop() {
    if (sampling_timer != nullptr) {
        (void)esp_timer_stop(sampling_timer);
    }
}

bool sampling_timer_consume_overflow() {
    portENTER_CRITICAL(&overflow_mux);
    const bool overflow = sample_request_overflow;
    sample_request_overflow = false;
    portEXIT_CRITICAL(&overflow_mux);

    return overflow;
}
