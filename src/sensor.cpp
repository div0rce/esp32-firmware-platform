#include "sensor.h"

#include <Arduino.h>

#include "app_config.h"
#include "sensor_math.h"

static AdcMovingAverage adc_filter;
static AdcRangeState adc_range_state = ADC_RANGE_NORMAL;

void sensor_init() {
    analogReadResolution(ADC_RESOLUTION_BITS);
    adc_moving_average_init(&adc_filter);
    adc_range_state = ADC_RANGE_NORMAL;
}

SensorSample read_sensor_sample(const SampleRequest& request) {
    const int raw = analogRead(ADC_PIN);
    const int filtered_raw = adc_moving_average_update(&adc_filter, raw);
    adc_range_state = adc_range_update(adc_range_state, filtered_raw);

    SensorSample sample;
    sample.timestamp_ms = request.timestamp_ms;
    sample.adc_raw = raw;
    sample.voltage_mv = adc_raw_to_mv(filtered_raw);
    sample.fault = fault_for_adc_range(adc_range_state);

    return sample;
}

bool sample_is_valid(const SensorSample& sample) {
    return sample.fault == FAULT_NONE;
}
