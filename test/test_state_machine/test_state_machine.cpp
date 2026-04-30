#include <unity.h>

#include "state_machine.h"

void test_boot_to_idle() {
    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, state_machine_next(SYSTEM_STATE_BOOT, APP_EVENT_BOOT_COMPLETE));
}

void test_normal_sample_transmit_cycle() {
    SystemState state = SYSTEM_STATE_IDLE;

    state = state_machine_next(state, APP_EVENT_SAMPLE_STARTED);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_SAMPLE, state);

    state = state_machine_next(state, APP_EVENT_TRANSMIT_STARTED);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_TRANSMIT, state);

    state = state_machine_next(state, APP_EVENT_TRANSMIT_COMPLETE);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, state);
}

void test_fault_overrides_any_state() {
    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, state_machine_next(SYSTEM_STATE_IDLE, APP_EVENT_FAULT_DETECTED));
    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, state_machine_next(SYSTEM_STATE_SAMPLE, APP_EVENT_FAULT_DETECTED));
    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, state_machine_next(SYSTEM_STATE_TRANSMIT, APP_EVENT_FAULT_DETECTED));
}

void test_fault_recovery_path() {
    SystemState state = SYSTEM_STATE_FAULT;

    state = state_machine_next(state, APP_EVENT_RECOVERY_REQUESTED);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_RECOVERY, state);

    state = state_machine_next(state, APP_EVENT_RECOVERY_COMPLETE);
    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, state);
}

void test_invalid_transition_holds_state() {
    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, state_machine_next(SYSTEM_STATE_IDLE, APP_EVENT_TRANSMIT_COMPLETE));
    TEST_ASSERT_EQUAL(SYSTEM_STATE_FAULT, state_machine_next(SYSTEM_STATE_FAULT, APP_EVENT_TRANSMIT_STARTED));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_boot_to_idle);
    RUN_TEST(test_normal_sample_transmit_cycle);
    RUN_TEST(test_fault_overrides_any_state);
    RUN_TEST(test_fault_recovery_path);
    RUN_TEST(test_invalid_transition_holds_state);
    return UNITY_END();
}
