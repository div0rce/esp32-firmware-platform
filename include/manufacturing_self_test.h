#pragma once

struct ManufacturingSelfTestResult {
    bool passed;
    int adc_raw;
    bool adc_raw_in_range;
};

ManufacturingSelfTestResult manufacturing_self_test_run();
