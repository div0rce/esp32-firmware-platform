#include "telemetry.h"

#include "app_config.h"
#include "firmware_hal.h"
#include "telemetry_format.h"

void telemetry_init() {
    firmware_hal::uart_begin(SERIAL_BAUD_RATE);
}

void telemetry_print_sample(SystemState state, const SensorSample& sample) {
    char buffer[TELEMETRY_BUFFER_LENGTH];

    TelemetryFields fields;
    fields.timestamp_ms = sample.timestamp_ms;
    fields.state = state;
    fields.adc_raw = sample.adc_raw;
    fields.voltage_mv = sample.voltage_mv;
    fields.fault = sample.fault;

    if (format_telemetry_packet(buffer, sizeof(buffer), &fields)) {
        firmware_hal::uart_println(buffer);
    }
}

void telemetry_print_fault_event(std::uint32_t timestamp_ms, FaultCode fault) {
    char buffer[TELEMETRY_BUFFER_LENGTH];

    if (format_fault_event(buffer, sizeof(buffer), timestamp_ms, fault)) {
        firmware_hal::uart_println(buffer);
    }
}

void telemetry_print_runtime_health(const RuntimeHealthFields& fields) {
    char buffer[TELEMETRY_BUFFER_LENGTH];

    if (format_runtime_health_event(buffer, sizeof(buffer), &fields)) {
        firmware_hal::uart_println(buffer);
    }
}

void telemetry_print_self_test_result(std::uint32_t timestamp_ms, const ManufacturingSelfTestResult& result) {
    char buffer[TELEMETRY_BUFFER_LENGTH];

    if (format_self_test_event(
        buffer,
        sizeof(buffer),
        timestamp_ms,
        result.passed,
        result.adc_raw,
        result.adc_raw_in_range,
        result.button_gpio_read_ok,
        result.button_level
    )) {
        firmware_hal::uart_println(buffer);
    }
}
