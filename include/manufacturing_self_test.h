#pragma once

struct ManufacturingSelfTestResult {
    bool passed;
    const char* firmware_name;
    const char* firmware_version;
    const char* firmware_target;
    int adc_raw;
    bool adc_raw_in_range;
    bool button_gpio_read_ok;
    bool button_level;
    bool app_state_init_ok;
    bool watchdog_init_ok;
    bool queue_create_ok;
    bool task_create_ok;
};

ManufacturingSelfTestResult manufacturing_self_test_run();
void manufacturing_self_test_complete(
    ManufacturingSelfTestResult* result,
    bool app_state_init_ok,
    bool watchdog_init_ok,
    bool queue_create_ok,
    bool task_create_ok
);
