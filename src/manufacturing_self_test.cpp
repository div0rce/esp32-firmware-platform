#include "manufacturing_self_test.h"

#include "app_config.h"
#include "firmware_hal.h"

ManufacturingSelfTestResult manufacturing_self_test_run() {
    const int adc_raw = firmware_hal::adc_read_raw(ADC_PIN);
    const bool adc_raw_in_range = adc_raw >= ADC_RAW_MIN && adc_raw <= ADC_RAW_MAX;
    bool button_level = false;
    const bool button_gpio_read_ok = firmware_hal::gpio_read_input_register(BUTTON_PIN, &button_level);

    ManufacturingSelfTestResult result;
    result.passed = adc_raw_in_range && button_gpio_read_ok;
    result.adc_raw = adc_raw;
    result.adc_raw_in_range = adc_raw_in_range;
    result.button_gpio_read_ok = button_gpio_read_ok;
    result.button_level = button_level;

    return result;
}
