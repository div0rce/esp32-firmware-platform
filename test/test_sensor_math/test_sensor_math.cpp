#include <unity.h>

#include "app_config.h"
#include "sensor_math.h"

void test_adc_raw_to_mv_bounds() {
    TEST_ASSERT_EQUAL(0, adc_raw_to_mv(ADC_RAW_MIN));
    TEST_ASSERT_EQUAL(ADC_REF_MV, adc_raw_to_mv(ADC_RAW_MAX));
    TEST_ASSERT_EQUAL(0, adc_raw_to_mv(-1));
    TEST_ASSERT_EQUAL(ADC_REF_MV, adc_raw_to_mv(ADC_RAW_MAX + 1));
}

void test_adc_raw_to_mv_midscale() {
    TEST_ASSERT_INT_WITHIN(1, ADC_REF_MV / 2, adc_raw_to_mv(ADC_RAW_MAX / 2));
}

void test_fault_for_adc_raw() {
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault_for_adc_raw(ADC_MIN_VALID - 1));
    TEST_ASSERT_EQUAL(FAULT_NONE, fault_for_adc_raw(ADC_MIN_VALID));
    TEST_ASSERT_EQUAL(FAULT_NONE, fault_for_adc_raw(ADC_MAX_VALID));
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault_for_adc_raw(ADC_MAX_VALID + 1));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_adc_raw_to_mv_bounds);
    RUN_TEST(test_adc_raw_to_mv_midscale);
    RUN_TEST(test_fault_for_adc_raw);
    return UNITY_END();
}
