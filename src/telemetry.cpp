#include "telemetry.h"

#include <Arduino.h>

#include "app_config.h"
#include "telemetry_format.h"

void telemetry_init() {
    Serial.begin(SERIAL_BAUD_RATE);
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
        Serial.println(buffer);
    }
}

void telemetry_print_fault_event(std::uint32_t timestamp_ms, FaultCode fault) {
    char buffer[TELEMETRY_BUFFER_LENGTH];

    if (format_fault_event(buffer, sizeof(buffer), timestamp_ms, fault)) {
        Serial.println(buffer);
    }
}
