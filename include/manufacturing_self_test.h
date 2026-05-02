#pragma once

struct ManufacturingSelfTestResult {
    bool passed;
    int adc_raw;
    bool adc_raw_in_range;
    bool button_gpio_read_ok;
    bool button_level;
};

ManufacturingSelfTestResult manufacturing_self_test_run();
