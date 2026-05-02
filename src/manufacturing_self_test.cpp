#include "manufacturing_self_test.h"

#include <Arduino.h>

#include "app_config.h"
#include "gpio_registers.h"

ManufacturingSelfTestResult manufacturing_self_test_run() {
    const int adc_raw = analogRead(ADC_PIN);
    const bool adc_raw_in_range = adc_raw >= ADC_RAW_MIN && adc_raw <= ADC_RAW_MAX;
    bool button_level = false;
    const bool button_gpio_read_ok = gpio_input_level_from_register(BUTTON_PIN, &button_level);

    ManufacturingSelfTestResult result;
    result.passed = adc_raw_in_range && button_gpio_read_ok;
    result.adc_raw = adc_raw;
    result.adc_raw_in_range = adc_raw_in_range;
    result.button_gpio_read_ok = button_gpio_read_ok;
    result.button_level = button_level;

    return result;
}
