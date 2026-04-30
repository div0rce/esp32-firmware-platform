#include <cstdint>
#include <limits>

#include <unity.h>

#include "time_utils.h"

void test_elapsed_ms_before_interval() {
    TEST_ASSERT_FALSE(elapsed_ms_u32(1099U, 1000U, 100U));
}

void test_elapsed_ms_at_interval() {
    TEST_ASSERT_TRUE(elapsed_ms_u32(1100U, 1000U, 100U));
}

void test_elapsed_ms_after_interval() {
    TEST_ASSERT_TRUE(elapsed_ms_u32(1250U, 1000U, 100U));
}

void test_elapsed_ms_handles_u32_rollover() {
    const std::uint32_t previous = std::numeric_limits<std::uint32_t>::max() - 10U;
    const std::uint32_t now = 20U;

    TEST_ASSERT_TRUE(elapsed_ms_u32(now, previous, 30U));
    TEST_ASSERT_FALSE(elapsed_ms_u32(now, previous, 32U));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_elapsed_ms_before_interval);
    RUN_TEST(test_elapsed_ms_at_interval);
    RUN_TEST(test_elapsed_ms_after_interval);
    RUN_TEST(test_elapsed_ms_handles_u32_rollover);
    return UNITY_END();
}
