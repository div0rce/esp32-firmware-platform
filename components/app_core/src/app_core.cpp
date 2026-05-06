#include "app_core.h"

#include "app_config.h"
#include "platform_hal.h"
#include "state_machine.h"
#include "telemetry_format.h"
#include "time_utils.h"

namespace {

bool due(uint32_t now_ms, uint32_t scheduled_ms) {
    return scheduled_ms == 0U || static_cast<int32_t>(now_ms - scheduled_ms) >= 0;
}

void publish_sample(AppCoreContext* context, uint32_t now_ms) {
    uint16_t raw = 0;
    FaultCode fault = FAULT_NONE;

    if (!platform_adc_read_raw(&raw)) {
        fault = FAULT_ADC_OUT_OF_RANGE;
        context->last_adc_raw = -1;
        context->last_voltage_mv = 0;
        context->adc_range_state = ADC_RANGE_LOW_FAULT;
    } else {
        const int filtered_raw = adc_moving_average_update(&context->adc_filter, raw);
        context->adc_range_state = adc_range_update(context->adc_range_state, filtered_raw);
        fault = fault_for_adc_range(context->adc_range_state);
        context->last_adc_raw = raw;
        context->last_voltage_mv = adc_raw_to_mv(filtered_raw);
    }

    context->fault = fault;
    context->has_sample = true;
    context->samples += 1U;
    context->state = state_machine_next(context->state, APP_EVENT_SAMPLE_STARTED);

    if (fault != FAULT_NONE) {
        context->state = state_machine_next(context->state, APP_EVENT_FAULT_DETECTED);
        platform_gpio_write(PLATFORM_GPIO_STATUS_LED, true);
    }

    TelemetryFields fields;
    fields.timestamp_ms = now_ms;
    fields.state = fault == FAULT_NONE ? SYSTEM_STATE_TRANSMIT : SYSTEM_STATE_FAULT;
    fields.adc_raw = context->last_adc_raw;
    fields.voltage_mv = context->last_voltage_mv;
    fields.fault = fault;

    char buffer[TELEMETRY_BUFFER_LENGTH];
    if (format_telemetry_packet(buffer, sizeof(buffer), &fields)) {
        (void)platform_uart_write_line(buffer);
        context->telemetry_packets += 1U;
    }

    if (fault == FAULT_NONE) {
        context->state = state_machine_next(context->state, APP_EVENT_TRANSMIT_STARTED);
        context->state = state_machine_next(context->state, APP_EVENT_TRANSMIT_COMPLETE);
        platform_gpio_write(PLATFORM_GPIO_STATUS_LED, false);
    }
}

}

void app_core_init(AppCoreContext* context) {
    if (context == nullptr) {
        return;
    }

    adc_moving_average_init(&context->adc_filter);
    context->adc_range_state = ADC_RANGE_NORMAL;
    context->state = state_machine_next(SYSTEM_STATE_BOOT, APP_EVENT_BOOT_COMPLETE);
    context->fault = FAULT_NONE;
    context->next_sample_ms = 0;
    context->next_telemetry_ms = 0;
    context->last_watchdog_kick_ms = 0;
    context->samples = 0;
    context->telemetry_packets = 0;
    context->fault_checks = 0;
    context->button_events = 0;
    context->last_adc_raw = 0;
    context->last_voltage_mv = 0;
    context->has_sample = false;
}

void app_core_tick(AppCoreContext* context, uint32_t now_ms) {
    if (context == nullptr) {
        return;
    }

    if (platform_button_read() && context->fault != FAULT_NONE) {
        context->button_events += 1U;
        context->fault = FAULT_NONE;
        context->state = state_machine_next(context->state, APP_EVENT_RECOVERY_REQUESTED);
        context->state = state_machine_next(context->state, APP_EVENT_RECOVERY_COMPLETE);
        platform_gpio_write(PLATFORM_GPIO_STATUS_LED, false);
    }

    if (due(now_ms, context->next_sample_ms)) {
        platform_trace_set(PLATFORM_TRACE_SAMPLE, true);
        publish_sample(context, now_ms);
        platform_trace_set(PLATFORM_TRACE_SAMPLE, false);
        context->next_sample_ms = now_ms + SAMPLE_PERIOD_MS;
    }

    context->fault_checks += 1U;
    context->last_watchdog_kick_ms = now_ms;
}
