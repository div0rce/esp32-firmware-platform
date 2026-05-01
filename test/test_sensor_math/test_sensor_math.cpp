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

void test_adc_runtime_range_cases() {
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault_for_adc_raw(0));
    TEST_ASSERT_EQUAL(FAULT_NONE, fault_for_adc_raw(200));
    TEST_ASSERT_EQUAL(FAULT_NONE, fault_for_adc_raw(2050));
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault_for_adc_raw(4095));
}

void test_moving_average_fills_then_rolls_window() {
    AdcMovingAverage filter;
    adc_moving_average_init(&filter);

    TEST_ASSERT_EQUAL(10, adc_moving_average_update(&filter, 10));
    TEST_ASSERT_EQUAL(15, adc_moving_average_update(&filter, 20));
    TEST_ASSERT_EQUAL(20, adc_moving_average_update(&filter, 30));
    TEST_ASSERT_EQUAL(25, adc_moving_average_update(&filter, 40));
    TEST_ASSERT_EQUAL(35, adc_moving_average_update(&filter, 50));
}

void test_moving_average_clamps_adc_bounds() {
    AdcMovingAverage filter;
    adc_moving_average_init(&filter);

    TEST_ASSERT_EQUAL(ADC_RAW_MIN, adc_moving_average_update(&filter, ADC_RAW_MIN - 20));
    TEST_ASSERT_EQUAL(ADC_RAW_MAX / 2, adc_moving_average_update(nullptr, ADC_RAW_MAX / 2));
    TEST_ASSERT_EQUAL(ADC_RAW_MAX, adc_moving_average_update(nullptr, ADC_RAW_MAX + 20));
}

void test_adc_low_fault_hysteresis() {
    AdcRangeState state = ADC_RANGE_NORMAL;

    state = adc_range_update(state, ADC_LOW_FAULT_ENTER - 1);
    TEST_ASSERT_EQUAL(ADC_RANGE_LOW_FAULT, state);

    state = adc_range_update(state, ADC_LOW_FAULT_CLEAR);
    TEST_ASSERT_EQUAL(ADC_RANGE_LOW_FAULT, state);

    state = adc_range_update(state, ADC_LOW_FAULT_CLEAR + 1);
    TEST_ASSERT_EQUAL(ADC_RANGE_NORMAL, state);
}

void test_adc_high_fault_hysteresis() {
    AdcRangeState state = ADC_RANGE_NORMAL;

    state = adc_range_update(state, ADC_HIGH_FAULT_ENTER + 1);
    TEST_ASSERT_EQUAL(ADC_RANGE_HIGH_FAULT, state);

    state = adc_range_update(state, ADC_HIGH_FAULT_CLEAR);
    TEST_ASSERT_EQUAL(ADC_RANGE_HIGH_FAULT, state);

    state = adc_range_update(state, ADC_HIGH_FAULT_CLEAR - 1);
    TEST_ASSERT_EQUAL(ADC_RANGE_NORMAL, state);
}

void test_fault_for_adc_range() {
    TEST_ASSERT_EQUAL(FAULT_NONE, fault_for_adc_range(ADC_RANGE_NORMAL));
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault_for_adc_range(ADC_RANGE_LOW_FAULT));
    TEST_ASSERT_EQUAL(FAULT_ADC_OUT_OF_RANGE, fault_for_adc_range(ADC_RANGE_HIGH_FAULT));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_adc_raw_to_mv_bounds);
    RUN_TEST(test_adc_raw_to_mv_midscale);
    RUN_TEST(test_fault_for_adc_raw);
    RUN_TEST(test_adc_runtime_range_cases);
    RUN_TEST(test_moving_average_fills_then_rolls_window);
    RUN_TEST(test_moving_average_clamps_adc_bounds);
    RUN_TEST(test_adc_low_fault_hysteresis);
    RUN_TEST(test_adc_high_fault_hysteresis);
    RUN_TEST(test_fault_for_adc_range);
    return UNITY_END();
}
