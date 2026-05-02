#include "manufacturing_self_test.h"

#include <Arduino.h>

#include "app_config.h"

ManufacturingSelfTestResult manufacturing_self_test_run() {
    const int adc_raw = analogRead(ADC_PIN);
    const bool adc_raw_in_range = adc_raw >= ADC_RAW_MIN && adc_raw <= ADC_RAW_MAX;

    ManufacturingSelfTestResult result;
    result.passed = adc_raw_in_range;
    result.adc_raw = adc_raw;
    result.adc_raw_in_range = adc_raw_in_range;

    return result;
}
