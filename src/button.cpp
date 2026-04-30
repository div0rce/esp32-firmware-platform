#include "button.h"

#include <Arduino.h>

#include "app_config.h"

static QueueHandle_t button_queue = nullptr;
static portMUX_TYPE overflow_mux = portMUX_INITIALIZER_UNLOCKED;
static bool button_queue_overflow = false;

static void IRAM_ATTR button_set_overflow_from_isr() {
    portENTER_CRITICAL_ISR(&overflow_mux);
    button_queue_overflow = true;
    portEXIT_CRITICAL_ISR(&overflow_mux);
}

static void IRAM_ATTR button_isr() {
    if (button_queue == nullptr) {
        button_set_overflow_from_isr();
        return;
    }

    ButtonEvent event;
    event.timestamp_ms = static_cast<std::uint32_t>(
        xTaskGetTickCountFromISR() * portTICK_PERIOD_MS
    );

    BaseType_t higher_priority_task_woken = pdFALSE;

    const BaseType_t result = xQueueSendFromISR(
        button_queue,
        &event,
        &higher_priority_task_woken
    );

    if (result != pdPASS) {
        button_set_overflow_from_isr();
    }

    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

void button_init(QueueHandle_t queue) {
    button_queue = queue;

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_isr, FALLING);
}

bool button_receive_event(ButtonEvent* event, TickType_t timeout_ticks) {
    if (event == nullptr || button_queue == nullptr) {
        return false;
    }

    return xQueueReceive(button_queue, event, timeout_ticks) == pdPASS;
}

bool button_consume_overflow() {
    portENTER_CRITICAL(&overflow_mux);
    const bool overflow = button_queue_overflow;
    button_queue_overflow = false;
    portEXIT_CRITICAL(&overflow_mux);

    return overflow;
}
