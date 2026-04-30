#include <unity.h>

#include "fault_state.h"

void test_state_strings() {
    TEST_ASSERT_EQUAL_STRING("BOOT", state_to_string(SystemState::BOOT));
    TEST_ASSERT_EQUAL_STRING("IDLE", state_to_string(SystemState::IDLE));
    TEST_ASSERT_EQUAL_STRING("SAMPLE", state_to_string(SystemState::SAMPLE));
    TEST_ASSERT_EQUAL_STRING("TRANSMIT", state_to_string(SystemState::TRANSMIT));
    TEST_ASSERT_EQUAL_STRING("FAULT", state_to_string(SystemState::FAULT));
    TEST_ASSERT_EQUAL_STRING("RECOVERY", state_to_string(SystemState::RECOVERY));
}

void test_fault_strings() {
    TEST_ASSERT_EQUAL_STRING("NONE", fault_to_string(FaultCode::NONE));
    TEST_ASSERT_EQUAL_STRING("ADC_OUT_OF_RANGE", fault_to_string(FaultCode::ADC_OUT_OF_RANGE));
    TEST_ASSERT_EQUAL_STRING("SENSOR_QUEUE_SEND_FAILED", fault_to_string(FaultCode::SENSOR_QUEUE_SEND_FAILED));
    TEST_ASSERT_EQUAL_STRING("BUTTON_QUEUE_SEND_FAILED", fault_to_string(FaultCode::BUTTON_QUEUE_SEND_FAILED));
    TEST_ASSERT_EQUAL_STRING("SAMPLE_TIMER_FAILED", fault_to_string(FaultCode::SAMPLE_TIMER_FAILED));
    TEST_ASSERT_EQUAL_STRING("WATCHDOG_TIMEOUT", fault_to_string(FaultCode::WATCHDOG_TIMEOUT));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_state_strings);
    RUN_TEST(test_fault_strings);
    return UNITY_END();
}
