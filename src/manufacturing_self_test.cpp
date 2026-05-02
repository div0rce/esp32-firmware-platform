#include "manufacturing_self_test.h"

#include "app_config.h"
#include "firmware_hal.h"

ManufacturingSelfTestResult manufacturing_self_test_run() {
    const int adc_raw = firmware_hal::adc_read_raw(ADC_PIN);
    const bool adc_raw_in_range = adc_raw >= ADC_RAW_MIN && adc_raw <= ADC_RAW_MAX;
    bool button_level = false;
    const bool button_gpio_read_ok = firmware_hal::gpio_read_input_register(BUTTON_PIN, &button_level);

    ManufacturingSelfTestResult result;
    result.passed = false;
    result.firmware_name = FIRMWARE_NAME;
    result.firmware_version = FIRMWARE_VERSION;
    result.firmware_target = FIRMWARE_TARGET;
    result.adc_raw = adc_raw;
    result.adc_raw_in_range = adc_raw_in_range;
    result.button_gpio_read_ok = button_gpio_read_ok;
    result.button_level = button_level;
    result.app_state_init_ok = false;
    result.watchdog_init_ok = false;
    result.queue_create_ok = false;
    result.task_create_ok = false;

    return result;
}

void manufacturing_self_test_complete(
    ManufacturingSelfTestResult* result,
    bool app_state_init_ok,
    bool watchdog_init_ok,
    bool queue_create_ok,
    bool task_create_ok
) {
    if (result == nullptr) {
        return;
    }

    result->app_state_init_ok = app_state_init_ok;
    result->watchdog_init_ok = watchdog_init_ok;
    result->queue_create_ok = queue_create_ok;
    result->task_create_ok = task_create_ok;
    result->passed = result->adc_raw_in_range
        && result->button_gpio_read_ok
        && result->app_state_init_ok
        && result->watchdog_init_ok
        && result->queue_create_ok
        && result->task_create_ok;
}
