#include "sensor.h"

#include <Arduino.h>

#include "app_config.h"
#include "sensor_math.h"

void sensor_init() {
    analogReadResolution(ADC_RESOLUTION_BITS);
}

SensorSample read_sensor_sample(const SampleRequest& request) {
    const int raw = analogRead(ADC_PIN);

    SensorSample sample;
    sample.timestamp_ms = request.timestamp_ms;
    sample.adc_raw = raw;
    sample.voltage_mv = adc_raw_to_mv(raw);
    sample.fault = fault_for_adc_raw(raw);

    return sample;
}

bool sample_is_valid(const SensorSample& sample) {
    return sample.fault == FAULT_NONE;
}
