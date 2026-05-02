#include "stress_mode.h"

#if defined(ENABLE_STRESS_MODE)

#include <freertos/task.h>

#include "app_config.h"
#include "button.h"
#include "sampling_timer.h"
#include "time_utils.h"

static bool is_recovery_window(std::uint32_t timestamp_ms) {
    return (timestamp_ms % STRESS_FAULT_CYCLE_MS) >= STRESS_FAULT_WINDOW_MS;
}

void stress_mode_init(StressModeState* state) {
    if (state == nullptr) {
        return;
    }

    state->last_recovery_inject_ms = 0;
    state->last_overflow_inject_ms = 0;
}

FaultCode stress_mode_forced_sample_fault(std::uint32_t timestamp_ms) {
    return is_recovery_window(timestamp_ms) ? FAULT_NONE : FAULT_ADC_OUT_OF_RANGE;
}

void stress_mode_delay_telemetry() {
    vTaskDelay(pdMS_TO_TICKS(STRESS_TELEMETRY_DELAY_MS));
}

void stress_mode_service(StressModeState* state, std::uint32_t now_ms, QueueHandle_t button_event_queue) {
    if (state == nullptr) {
        return;
    }

    if (
        state->last_overflow_inject_ms == 0U
        || elapsed_ms_u32(now_ms, state->last_overflow_inject_ms, STRESS_OVERFLOW_INJECT_PERIOD_MS)
    ) {
        button_stress_mark_overflow();
        sampling_timer_stress_mark_overflow();
        state->last_overflow_inject_ms = now_ms;
    }

    if (
        button_event_queue != nullptr
        && is_recovery_window(now_ms)
        && (
            state->last_recovery_inject_ms == 0U
            || elapsed_ms_u32(now_ms, state->last_recovery_inject_ms, STRESS_RECOVERY_INJECT_PERIOD_MS)
        )
    ) {
        ButtonEvent event;
        event.timestamp_ms = now_ms;
        (void)xQueueSend(button_event_queue, &event, 0);
        state->last_recovery_inject_ms = now_ms;
    }
}

#endif
